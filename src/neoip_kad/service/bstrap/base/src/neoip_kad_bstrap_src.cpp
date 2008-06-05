/*! \file
    \brief Definition of the kad_bstrap_src_t

\par Brief Description
- the kad_bstrap_src_t unify 3 other sources
  -# nslan_src which query a nslan to get the kad_peer_t from the lan
  -# dfile_src which uses a list of ipport_addr_t dynamically saved at the end of last kad_peer_t run
  -# sfile_src which uses a static list of ipport_addr_t
- the sfile_src is used only after a given delay to reduce the load on those hosts
- the nslan_src allows to run between hosts of the same lan without being connected to the 
  internet.
- It provides a negative cache system. aka if a kad_addr_t it pushed in the 
  negative cache, it won't be notified for a given delay tunable by profile

\par Implementation Notes
- the last item produced by each source is stored in a *_last_item variable
- there is a function want_full_item() which always try to keep all *_last_item full
- In case of empty source, e.g. no item in sfile_src, a plain application would result in 
  querying all the time the source. aka using 100%cpu for nothing
  - to avoid this, 2 states are used 
  -# _got_item which is true IIF the source already provided non null item
  -# _is_empty which is true IIF the source have been declared empty
- In case all the elements of a given source are all in the negative cache, the 
  100% CPU case could happen too.
  - to avoid this, if a source return an item which is in the negative cache
    a _noquery_timeout is started, and this source wont be query until this 
    delay expire.
- To deliver an address, the kad_bstrap_src_t uses a round robbin between the 3
  source.

\par Possible Improvement - bootstrap from bt http tracker
- to bootstrap from bt http tracker (implemented in bt_tracker_client_t)
- this allows to use an existing infrastructure of all the deployed bt http trackers
  - thus a bootstrap without central servers
- implement this in a ipport_srcbt_t class. 
  - similar to the ipport_srclist_t class.
- some issue with the fact one may have to use swarm which are used to actually
  track bt clients. 
  - so sending valid bt_tracker_req_t
  - maybe with some tricks to recognize other kad_peer_t amount the bt clients.
    - unclear as with the bt_tracker_req_t::compact() stuff there is no room to 
      pass any other information beside the ipport_addr_t
      
*/

/* system include */
#include <stdlib.h>
/* local include */
#include "neoip_kad_bstrap_src.hpp"
#include "neoip_kad_bstrap_profile.hpp"
#include "neoip_kad_bstrap_file.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_ipport_strlist_src.hpp"
#include "neoip_nslan_rec_src.hpp"
#include "neoip_nslan.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_serial.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_bstrap_src_t::kad_bstrap_src_t()		throw()
{
	// zero some field
	sfile_src	= NULL;
	dfile_src	= NULL;
	nslan_src	= NULL;
	negcache	= NULL;
	callback	= NULL;
	delivery_rrobin	= 0;
}

/** \brief Desstructor
 */
kad_bstrap_src_t::~kad_bstrap_src_t()		throw()
{
	// free the negcache if needed
	nipmem_zdelete negcache;
	// free sfile_src if needed
	nipmem_zdelete sfile_src;
	// free dfile_src if needed
	nipmem_zdelete dfile_src;
	// free nslan_src if needed
	nipmem_zdelete nslan_src;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
kad_bstrap_src_t &	kad_bstrap_src_t::set_profile(const kad_bstrap_src_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == kad_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation for this object
 * 
 * @param dfile_name	contain the filename of the dynamic source, if empty() no dynamic source
 * @param sfile_name	contain the filename of the static source, if empty() no static source
 * @param nslan_peer	contain the nslan_peer_t on which the nslan source operate
 * @param nslan_keyid	the keyid to query to find other peer record on the nslan_peer_t
 * 
 * @return a kad_err_t
 */
kad_err_t	kad_bstrap_src_t::start(const file_path_t &dynfile_path, const file_path_t &fixfile_path
					, nslan_peer_t *nslan_peer, const nslan_keyid_t &nslan_keyid
					, kad_bstrap_src_cb_t *callback, void *userptr)	throw()
{
	inet_err_t		inet_err;
	// sanity check - the kad_bstrap_src_t MUST NOT be already started
	DBG_ASSERT( !nslan_src );
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// set some parameter
	want_more	= false;

	// init the negative cache
	negcache	= nipmem_new expireset_t<kad_addr_t>(profile.negcache_timeout());

#if 1	// TODO to reenable - disabled nslan_rec_src_t to test the kad_bstrap_src_t from file only
	// create the nslan_src
	nslan_src	= nipmem_new nslan_rec_src_t();
	inet_err	= nslan_src->start(nslan_peer, nslan_keyid, this, NULL);	
	if( inet_err.failed() )	return kad_err_from_inet(inet_err);
	// set internal variable for nslan_src
	nslan_got_item	= false;
	nslan_is_empty	= false;
#else
	nslan_got_item	= false;
	nslan_is_empty	= true;
#endif

#if 1	// TODO to reenable - disabled to debug
	// create the dfile_src
	dfile_src	= nipmem_new ipport_strlist_src_t();
	if( !dynfile_path.is_null() && !file_stat_t(dynfile_path).is_null() ){
		inet_err = dfile_src->append_from_file(dynfile_path.to_string());
	}else{
		inet_err = inet_err_t::OK;
	}
	if( inet_err.succeed())		inet_err = dfile_src->start(this, NULL);
	if( inet_err.failed() )		return kad_err_from_inet(inet_err);
	// set internal variable for dfile_src
	dfile_got_item	= false;
	dfile_is_empty	= false;
	// log to debug
	KLOG_DBG("dfile_src=" << *dfile_src);
#else
	dfile_got_item	= false;
	dfile_is_empty	= true;
#endif

	// set internal variable for sfile_src
	sfile_got_item	= false;
	sfile_is_empty	= false;
	// start the timeout for the creation of the sfile_src
	// - it is created later to reduce the load of the static bootstrappers
	if( !fixfile_path.is_null() && !file_stat_t(fixfile_path).is_null() ){
		this->sfile_name= fixfile_path.to_string();
		sfile_create_timeout.start(profile.sfile_creation_delay(), this, NULL);	
	}else{
		sfile_is_empty = true;
	}


	// do a want_full_item() to keep the item of each source full
	want_full_item();
	
	// return no error
	return kad_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       global timeout_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the sfile_create_timeout expire
 */
bool 	kad_bstrap_src_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// route the callback depending of the timeout_t which triggered it
	if( &sfile_create_timeout == &cb_timeout )	return sfile_create_timeout_cb(userptr, cb_timeout);
	if( &nslan_noquery_timeout == &cb_timeout )	return nslan_noquery_timeout_cb(userptr, cb_timeout);
	if( &dfile_noquery_timeout == &cb_timeout )	return dfile_noquery_timeout_cb(userptr, cb_timeout);
	if( &sfile_noquery_timeout == &cb_timeout )	return sfile_noquery_timeout_cb(userptr, cb_timeout);
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       creation of sfile_src
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the sfile_create_timeout expire
 */
bool 	kad_bstrap_src_t::sfile_create_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// create the static file source
	sfile_create();		
	// return tokeep
	return true;
}

/** \brief Create the static file source
 */
kad_err_t	kad_bstrap_src_t::sfile_create()	throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter. creating the sfile_src.");
	// sanity check - sfile_src MUST be NULL
	DBG_ASSERT( !sfile_src );
	DBG_ASSERT( !sfile_name.empty() );

	// stop the timeout
	sfile_create_timeout.stop();

	// create the sfile_src
	sfile_src	= nipmem_new ipport_strlist_src_t();
	if( !sfile_name.empty() )	inet_err = sfile_src->append_from_file(sfile_name);
	else				inet_err = inet_err_t::OK;
	if( inet_err.succeed() )	inet_err = sfile_src->start(this, NULL);
	if( inet_err.failed() )		return kad_err_from_inet(inet_err);
	// log to debug
	KLOG_DBG("sfile_src=" << *sfile_src);

	// do a want_full_item() to keep the item of each source full
	want_full_item();
	
	// return no error
	return kad_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     nslan source
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref nslan_rec_src_t when to notify an nslan_rec_t
 */
bool kad_bstrap_src_t::neoip_nslan_rec_src_cb(void *cb_userptr, nslan_rec_src_t &cb_nslan_rec_src
		, const nslan_rec_t &nslan_rec, const ipport_addr_t &remote_oaddr)	throw()
{
	datum_t		payload	= nslan_rec.get_payload();
	serial_t	serial(payload.get_data(), payload.get_len());
	kad_addr_t	kad_addr;
	// log to debug
	KLOG_DBG("enter notified record=" << nslan_rec << " remote_oaddr=" << remote_oaddr);

	// if the notified item is not null, set _got_item to true
	if( !nslan_rec.is_null() )			nslan_got_item = true;
	
	// if it is the end-of-list and the source never got any item, set _is_empty to true
	if( nslan_rec.is_null() && !nslan_got_item )	nslan_is_empty = true;	

	// if it is end-of-list, silently ignore the reported item
	if( nslan_rec.is_null() )			return true;

	// parse the record to extract the kad_addr_t from it
	try{
		serial >> kad_addr;
	}catch(serial_except_t &e){
		KLOG_ERR("Cant parse incoming nslan record payload due to " << e.what() );
		return true;
	}
	
	// if the ip address in kad_addr_t is ANY, use the one from remote_oaddr
	// - NOTE: this allow the publisher on this nslan_rec_t to publish a listen_oaddr_lview in it
	if( kad_addr.oaddr().ipaddr().is_any() )	kad_addr.oaddr().ipaddr(remote_oaddr.ipaddr());

	// log to debug
	KLOG_DBG("kad addr=" << kad_addr);

	// sanity check - the kad_addr_t MUST be fully_qualified
	DBGNET_ASSERT(kad_addr.is_fully_qualified());

	// if this item is in the negcache, discard it and start the _noquery_timeout for this source
	if( negcache->contain(kad_addr) ){
		nslan_noquery_timeout.start(profile.nslan_noquery_timeout(), this, NULL);
		return true;
	}

	// if the nslan_last_item is not null, push it in the tobedelivered_db
	if( !nslan_last_item.is_null() )	tobedelivered_db.push_back(nslan_last_item);
	// copy the notified kad_addr_t in the item
	nslan_last_item	= kad_addr;
	// if the caller want_more, do a delivery
	if( want_more && !delivery_zerotimer.size() )	delivery_zerotimer.append(this, NULL);

	// return 'tokeep'
	return true;
}

/** \brief callback called when the nslan_noquery_timeout expire
 */
bool 	kad_bstrap_src_t::nslan_noquery_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// sanity check - the cb_timeout MUST be nslan_noquery_timeout
	DBG_ASSERT( &cb_timeout == &nslan_noquery_timeout );
	// stop the timeout_t
	nslan_noquery_timeout.stop();
	// ask for more to this source
	nslan_src->get_more();
	// return tokeep
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     global ipport_strlist_src callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ipport_strlist_src_t when to notify an ipport_addr_t
 */
bool kad_bstrap_src_t::neoip_ipport_strlist_src_cb(void *cb_userptr
					, ipport_strlist_src_t &cb_ipport_strlist_src
					, const ipport_addr_t &ipport_addr)	throw()
{
	// log to debug
	KLOG_DBG("enter ipport_addr=" << ipport_addr);
	// build a fake kad_addr_t from the ipport_addr_t of the source
	// - it is ok as the destination kad_listener will take the first kad_peer_t 
	//   to match the packet. 
	// - here we aims for bootstrap so any kad_peer_t of the realm will do (aka not one in particular)
	kad_addr_t	kad_addr(ipport_addr, kad_peerid_t());
	// route the callback depending of if it is dfile_src or sfile_src
	if( dfile_src == &cb_ipport_strlist_src )	return dfile_src_cb(kad_addr);
	if( sfile_src == &cb_ipport_strlist_src )	return sfile_src_cb(kad_addr);
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Dynamic file source
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by dfile_src when to notify an ipport_addr_t
 */
bool kad_bstrap_src_t::dfile_src_cb(const kad_addr_t &kad_addr)	throw()
{
	// log to debug
	KLOG_DBG("enter notified kad_addr=" << kad_addr);

	// if the notified item is not null, set _got_item to true
	if( !kad_addr.is_null() )			dfile_got_item = true;
	
	// if it is the end-of-list and the source never got any item, set _is_empty to true
	if( kad_addr.is_null() && !dfile_got_item )	dfile_is_empty = true;

	// if it is the end-of-list and the sfile_src is not yet started, start it now
	// - thus if all address from the dynamic file source have been tried before the 
	//   sfile_create_timeout expires, the sfile_src will be used immediatly
	// - NOTE: special case for dfile_src
	if( kad_addr.is_null() && !sfile_src && !sfile_is_empty )		sfile_create();

	// if it is end-of-list, silently ignore the reported item
	if( kad_addr.is_null() )	return true;

	// if this item is in the negcache, discard it and start the _noquery_timeout for this source
	if( negcache->contain(kad_addr) ){
		dfile_noquery_timeout.start(profile.dfile_noquery_timeout(), this, NULL);
		return true;
	}

	// if the dfile_last_item is not null, push it in the tobedelivered_db
	if( !dfile_last_item.is_null() )	tobedelivered_db.push_back(dfile_last_item);
	// copy the notified kad_addr_t in the item
	dfile_last_item	= kad_addr;
	// if the caller want_more, do a delivery
	if( want_more && !delivery_zerotimer.size() )	delivery_zerotimer.append(this, NULL);

	// return 'tokeep'
	return true;
}

/** \brief callback called when the dfile_noquery_timeout expire
 */
bool 	kad_bstrap_src_t::dfile_noquery_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// sanity check - the cb_timeout MUST be dfile_noquery_timeout
	DBG_ASSERT( &cb_timeout == &dfile_noquery_timeout );
	// stop the timeout_t
	dfile_noquery_timeout.stop();
	// ask for more to this source
	dfile_src->get_more();
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Static file source
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by dfile_src when to notify an ipport_addr_t
 */
bool kad_bstrap_src_t::sfile_src_cb(const kad_addr_t &kad_addr)	throw()
{
	// log to debug
	KLOG_DBG("enter notified kad_addr=" << kad_addr);

	// if the notified item is not null, set _got_item to true
	if( !kad_addr.is_null() )			sfile_got_item = true;
	
	// if it is the end-of-list and the source never got any item, set _is_empty to true
	if( kad_addr.is_null() && !sfile_got_item )	sfile_is_empty = true;
		
	// if it is end-of-list, silently ignore the reported item
	if( kad_addr.is_null() )			return true;

	// if this item is in the negcache, discard it and start the _noquery_timeout for this source
	if( negcache->contain(kad_addr) ){
		sfile_noquery_timeout.start(profile.sfile_noquery_timeout(), this, NULL);
		return true;
	}

	// if the sfile_last_item is not null, push it in the tobedelivered_db
	if( !sfile_last_item.is_null() )		tobedelivered_db.push_back(sfile_last_item);
	// copy the notified kad_addr_t in the item
	sfile_last_item	= kad_addr;
	// if the caller want_more, do a delivery
	if( want_more && !delivery_zerotimer.size() )	delivery_zerotimer.append(this, NULL);

	// return 'tokeep'
	return true;
}

/** \brief callback called when the sfile_noquery_timeout expire
 */
bool 	kad_bstrap_src_t::sfile_noquery_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// sanity check - the cb_timeout MUST be sfile_noquery_timeout
	DBG_ASSERT( &cb_timeout == &sfile_noquery_timeout );
	// stop the timeout_t
	sfile_noquery_timeout.stop();
	// ask for more to this source
	sfile_src->get_more();
	// return tokeep
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        Misc Internal function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Main function to deliver the items
 * 
 * - WARNING: this function may delete the object itself
 *   - so dont use it after this function
 * 
 * @return a 'tokeep'
 */
bool kad_bstrap_src_t::do_delivery()	throw()
{
	kad_addr_t	kad_addr;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - want_more MUST be true
	DBG_ASSERT( want_more );
	
	/*************** Deliver the tobedelivered_db	***********************/
	// if tobedelivered_db is not empty, deliver one item from it
	if( !tobedelivered_db.empty() ){
		// get the first kad_addr_t in the tobedelived_db
		kad_addr	= tobedelivered_db.front();
		tobedelivered_db.pop_front();
		// clear the want_more before notifing the callback
		want_more	= false;		
		// notify the callback of the current address
		return notify_callback(kad_addr);
	}


	/*************** Deliver from the items	***********************/
	// nullify the kad_addr
	kad_addr	= kad_addr_t();
	// try to find a non null item and copy it in kad_addr
	for(size_t i = 0; i < 3 && kad_addr.is_null(); i++ ){
		// log to debug
		KLOG_DBG("delivery_rrobin=" << delivery_rrobin);
		switch(delivery_rrobin){
		/******* 0 means nslan_src	*******************************/
		case 0:		if( nslan_last_item.is_null() )		break;
				KLOG_DBG("deliver from nslan_src=" << nslan_last_item);
				kad_addr	= nslan_last_item;
				nslan_last_item	= kad_addr_t();
				break;	
		/******* 1 means dfile_src	*******************************/
		case 1:		if( dfile_last_item.is_null() )		break;
				KLOG_DBG("deliver from dfile_src=" << dfile_last_item);
				kad_addr	= dfile_last_item;
				dfile_last_item	= kad_addr_t();
				break;	
		/******* 2 means sfile_src	*******************************/
		case 2:		if( sfile_last_item.is_null() )		break;
				KLOG_DBG("deliver from sfile_src=" << sfile_last_item);
				kad_addr	= sfile_last_item;
				sfile_last_item	= kad_addr_t();
				break;	
		default:	DBG_ASSERT(0);
		}
 		// increase and potentially warp around the round robbin
 		if( delivery_rrobin < 2 )	delivery_rrobin++;
 		else				delivery_rrobin = 0;
	}
	// if no item has been found, exit
	if( kad_addr.is_null() )	return true;

	// clear the want_more before notifing the callback
	want_more	= false;
	// notify the callback of the current address
	return notify_callback(kad_addr);
}

/** \brief Express the will to fill all item - aka if a item is not full
 *         do a get_more() on the corresponding source
 */
void kad_bstrap_src_t::want_full_item()	throw()
{
	// ask for more to nslan_src if needed
	if( nslan_last_item.is_null() && !nslan_is_empty && !nslan_noquery_timeout.is_running() )
		nslan_src->get_more();
	// ask for more to dfile_src if needed
	if( sfile_src && sfile_last_item.is_null() && !sfile_is_empty && !sfile_noquery_timeout.is_running() )
		sfile_src->get_more();
	// ask for more to sfile_src if needed
	if( dfile_last_item.is_null() && !dfile_is_empty && !dfile_noquery_timeout.is_running() )
		dfile_src->get_more();
}

/** \brief return true if some item are immediatly available, false otherwise
 */
bool	kad_bstrap_src_t::item_are_available()	throw()
{
	// test all items
	if( !nslan_last_item.is_null() )	return true;
	if( !dfile_last_item.is_null() )	return true;
	if( !sfile_last_item.is_null() )	return true;
	// test the tobedelivered_db
	if( tobedelivered_db.size() > 0 )	return true;
	// if this point is reached, no item are immediatly available
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       delivery_zerotimer callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - NOTE: this function is used IIF the caller do a get_more() and some items
 *   are available.
 */
bool	kad_bstrap_src_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// if the caller no more want more, return immediatly
	// - this may happen in some race case e.g. the caller do get_more(), the
	//   zerotimer_t is launched, some delivery is made thru a source callback,
	//   the caller no more want_more after that, and then the zerotimer_t
	//   callback is notified.
	if( !want_more )	return true;
	
	// call the main deliver function
	// NOTE: this function may delete the object, dont use it beyong this point
	return do_delivery();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Misc function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Warn the kad_bstrap_src_t that more addresses are wished
 * 
 * - it may be called several times in a row, is the same as calling it only once
 *   - it just indicate the wish of the caller to get_more()
 * - NOTE: this is ok to use it during a neoip_kad_bstrap_src_cb_t callback 
 */
void	kad_bstrap_src_t::get_more()		throw()
{
	// set the want_more
	want_more	= true;

	// do a want_full_item() to keep the item of each source full
	want_full_item();

	// if some items are available, do a delivery_zerotimer
	if( item_are_available() )	delivery_zerotimer.append(this, NULL);
}

/** \brief Push back this address into the address source such as it will be 
 *         the next address notified in the callback.
 * 
 * - NOTE: this is ok to use it during a neoip_kad_bstrap_src_cb_t callback 
 */
void	kad_bstrap_src_t::push_back(const kad_addr_t &kad_addr)	throw()
{
	// log to debug
	KLOG_DBG("enter kad_addr=" << kad_addr);
	// push the kad_addr_t at the BEGINING of tobedelivered_db
	tobedelivered_db.push_front(kad_addr);
}


/** \brief Push this address in the negative cache
 * 
 * - NOTE: this is ok to use it during a neoip_kad_bstrap_src_cb_t callback 
 */
void	kad_bstrap_src_t::push_negcache(const kad_addr_t &kad_addr, const delay_t &delay)	throw()
{
	// log to debug
	KLOG_DBG("enter kad_addr=" << kad_addr << " for " << delay);
	// put this kad_addr in the negcache for delay
	negcache->update(kad_addr, delay);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the kad_addr_t
 * 
 * @return a tokeep
 */
bool kad_bstrap_src_t::notify_callback(const kad_addr_t &kad_addr)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_bstrap_src_cb(userptr, *this, kad_addr);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



/*! \file
    \brief Definition of the nslan_rec_src_t

- TODO BUG BUG FIXME
  - the nslan_rec_db should be a expire_list of some sort, currently the nslan_rec_t
    ttl is not respected when pushed in this list

*/

/* system include */
#include <stdlib.h>
/* local include */
#include "neoip_nslan_rec_src.hpp"
#include "neoip_nslan.hpp"
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
nslan_rec_src_t::nslan_rec_src_t()		throw()
{
	// zero some fields
	nslan_query	= NULL;
}

/** \brief Desstructor
 */
nslan_rec_src_t::~nslan_rec_src_t()		throw()
{
	// stop the query if it is running
	if( nslan_query )	nipmem_delete	nslan_query;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation for this object
 * 
 * @param nslan_srv	the nslan_srv_t on which to operate 
 *                      NOTE: it is STILL owned by the caller. up to it not to delete it
 *                      during the whole life of this object.
 */
inet_err_t	nslan_rec_src_t::start(nslan_peer_t *nslan_peer, const nslan_keyid_t &keyid
					, nslan_rec_src_cb_t *callback, void *userptr)	throw()
{
	// sanity check - nslan_query MUST be NULL
	DBG_ASSERT( !nslan_query );
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;
	this->nslan_peer	= nslan_peer;
	this->keyid		= keyid;

	// set some parameter
	want_more		= false;
	// return no error
	return inet_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Misc function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Warn the nslan_rec_src_t that more items are wished
 * 
 * - it may be called several times in a row, is the same as calling it only once
 *   - it just indicate the wish of the caller to get_more()
 * - NOTE: this is ok to use it during a neoip_nslan_rec_src_cb_t callback 
 */
void	nslan_rec_src_t::get_more()		throw()
{
	// if the query is not yet launched, do it here
	if( nslan_query == NULL )
		nslan_query = nipmem_new nslan_query_t(nslan_peer, keyid, delay_t::NEVER, this, NULL);	

	// set the want_more
	want_more	= true;

	// launch the delivery process with a zerotimer_t
	zerotimer.append(this, NULL);
}

/** \brief Push back this address into the address source such as it will be 
 *         the next address notified in the callback.
 * 
 * - NOTE: this is ok to use it during a neoip_nslan_rec_src_cb_t callback 
 */
void	nslan_rec_src_t::push_back(const nslan_rec_t &nslan_rec, const ipport_addr_t &src_addr)	throw()
{
	// push the record at the begining of nslan_rec_db
	nslan_rec_db.push_front( std::make_pair(nslan_rec, src_addr) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     nslan_query callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when nslan_query_t has an event to report
 */
bool nslan_rec_src_t::neoip_nslan_query_cb(void *cb_userptr, nslan_query_t &nslan_query
					, const nslan_event_t &nslan_event)	throw()
{
	// log to debug
	KLOG_DBG("event = " << nslan_event );
	// sanity check - check the nslan_event is query_ok()
	DBG_ASSERT( nslan_event.is_query_ok() );
	// sanity check - as the query is without timeout, the event MUST be is_got_record()
	DBG_ASSERT( nslan_event.is_got_record() );
	
	// create an alias to the notified_record
	ipport_addr_t		src_addr;
	const nslan_rec_t &	nslan_rec	= nslan_event.get_got_record(&src_addr);

	// check if the record is already in the nslan_rec_db
	std::list<std::pair<nslan_rec_t, ipport_addr_t> >::iterator	iter;
	for( iter = nslan_rec_db.begin(); iter != nslan_rec_db.end(); iter++ ){
		// if the nslan_rec_t and src_addr match, skip this one
		if(iter->first == nslan_rec && iter->second == src_addr)	break;
	}
	// if the record is already in the nslan_rec_db, discard it
	if( !nslan_rec_db.empty() && iter != nslan_rec_db.end() )	return true;
	
	// put the record at the end of the list
	nslan_rec_db.push_back( std::make_pair(nslan_rec, src_addr) );
	
	// if the caller is currently want_more, launch a delivery
	// NOTE: this function may delete the object, dont use it beyong this point	
	if( want_more )	return do_delivery();

	// return tokeep
	return true;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	nslan_rec_src_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw()
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
//                        Internal function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Main function to deliver the items
 * 
 * @return a 'tokeep'
 */
bool	nslan_rec_src_t::do_delivery()	throw()
{
	// sanity check - want_more MUST be true
	DBG_ASSERT( want_more );
	
	// If nslan_rec_db is empty, return tokeep now
	if( nslan_rec_db.empty() )	return true;

	// get the front record to deliver from the nslan_rec_db
	nslan_rec_t	nslan_rec	= nslan_rec_db.front().first;
	ipport_addr_t	src_addr	= nslan_rec_db.front().second;
	// remove the front record from the nslan_rec_db
	nslan_rec_db.pop_front();

	// clear the want_more
	want_more	= false;

	// notify the callback of the current address
	return notify_callback(nslan_rec, src_addr);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	nslan_rec_src_t::notify_callback(const nslan_rec_t &nslan_rec
					, const ipport_addr_t &src_addr)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_nslan_rec_src_cb(userptr, *this, nslan_rec, src_addr);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



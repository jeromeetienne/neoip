/*! \file
    \brief Definition of the \ref kad_bstrap_t

- TODO BUG: a kad_bstrap_cand_t may be launched toward a kad_addr_t which is already in 
  the kbucket.
  - e.g. when launching 2 neoip-router on a lan, them will launch candidate to each
    other until it times out...
  - waste of rescource

- from the paper, "To join the network, a node u must have a contact to an already participating
  node w. u inserts w into the appropriate k-bucket. u then performs a node lookup
  for its own node ID. Finally, u refreshes all k-buckets further away than its closest
  neighbor. During the refreshes, u both populates its own k-buckets and inserts
  itself into other nodes’ k-buckets as necessary."

\par About stopping criteria
- It is handled in 3 sequential stages: which are mutually exclusive 
  1. multpub: stop if there is multiple public addresses in the kbucket
  2. multany: stop if there is multiple addresses (public or not) in the kbucket
  3. singany: stop if there is at least one address in the kbucket
  - obviously the address of the local kad_peer_t is not taken into account here
- each stage is valid only during a delay_t from profile
  1. multpub: stage is valid only during the first profile.stop_multpub_delay() of the kad_bstrap_t
  2. multany: stage is valid only during the first profile.stop_multany_delay() of the kad_bstrap_t
  3. singany: stage is valid only during the first profile.stop_singany_delay() of the kad_bstrap_t
  - if the kad_bstrap_t is older than profile.stop_singany_delay(), the kad_bstrap_t stops
    no matter the number of peers. obviously this delay_t has to be configured to be very
    long. as it is supposed to stop when pretty sure you can bootstrap
- additionnaly the profile contains the number of peer requires for each stage.
- Netsplit: one danger in bootstrapping is to have a part of the network bootstrapping
  with each other, and another part doing the same. but each part not seeing each other.
  - this result in 2 distincts networks
  - thus record published on one network wont be queriable on the other network
- the purpose of those various stage is to ensure to get public address when 
  possible and to get public address if possible

\par Implementation Notes
- the kad_peer_t is useable immediatly, no need to wait for the bootstrapping to complete.
  - obviously the GET records may return NOTFOUND when the record is actually 
    present on other kad's nodes... but it is temporary until the bootstrapping 
    is complete
  - the PUT record will works immediatly without any issue.
- the kad_bstrap_t launch at most profile.cand_nb_concurrent() kad_bstrap_cand_t
  - each of them on a different destination kad_addr_t from the kad_bstrap_src_t
- if the kad_addr_t from the kad_bstrap_src_t is garanteed to be the local
  peer, it is discarded.

*/

/* system include */
/* local include */
#include "neoip_kad_bstrap.hpp"
#include "neoip_kad_bstrap_cand.hpp"
#include "neoip_kad_bstrap_src.hpp"
#include "neoip_kad_bstrap_file.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_kbucket.hpp"
#include "neoip_kad_listener.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor by default
 */
kad_bstrap_t::kad_bstrap_t()	throw()
{
	// zero some fields
	kad_peer	= NULL;
	bstrap_src	= NULL;
}

/** \brief Destructor by default
 */
kad_bstrap_t::~kad_bstrap_t()	throw()
{
	// delete the kad_bstrap_src_t if needed
	nipmem_zdelete	bstrap_src;
	// close all pending kad_bstrap_cand_t
	while( !cand_db.empty() )	nipmem_delete	cand_db.front();	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
kad_bstrap_t &	kad_bstrap_t::set_profile(const kad_bstrap_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == kad_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
kad_err_t	kad_bstrap_t::start(kad_peer_t *kad_peer, kad_bstrap_cb_t *callback, void *userptr)	throw()
{
	kad_listener_t *kad_listener	= kad_peer->get_listener();
	nslan_peer_t *	nslan_peer	= kad_listener->nslan_peer();
	kad_err_t	kad_err;
	// copy the parameter
	this->kad_peer	= kad_peer;
	this->callback	= callback;
	this->userptr	= userptr;
	
	// set the start_date - it is used in the stopping criteria
	start_date	= date_t::present();

	// compute the filenames for the kad_bstrap_src_t
	file_path_t	dynfile_path	= kad_bstrap_file_t::dynfile_path(kad_peer);
	file_path_t	fixfile_path	= kad_bstrap_file_t::fixfile_path(kad_peer);
	// log to debug
	KLOG_ERR("dynfile_path=" << dynfile_path << " fixfile_path=" << fixfile_path);
	// init the kad_bstrap_src
	nslan_keyid_t	nslan_keyid	= kad_peer->get_realmid().to_canonical_string();
	bstrap_src	= nipmem_new kad_bstrap_src_t();
	kad_err		= bstrap_src->set_profile(profile.src()).start(dynfile_path, fixfile_path
							, nslan_peer, nslan_keyid, this, NULL);
	if( kad_err.failed() )	return kad_err;
	
	// ask for more to the bstrap_src - to start the bootstrapping
	bstrap_src->get_more();	

	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on a kad_bstrap_cand_t with a matching kad_addr_t or NULL if no matches
 */
kad_bstrap_cand_t *	kad_bstrap_t::cand_by_kad_addr(const kad_addr_t &kad_addr)	const throw()
{
	// go thru the cand_db to know if this remote_addr is already used as remote_addr
	std::list<kad_bstrap_cand_t *>::const_iterator	iter;
	for(iter = cand_db.begin(); iter != cand_db.end() ; iter++){
		kad_bstrap_cand_t *	bstrap_cand	= *iter;	
		// if this candidate already use this kad_addr_t, leave the loop
		if( kad_addr == bstrap_cand->get_remote_addr() )	return bstrap_cand;
	}
	// if this point is reached, there is no matches, so return NULL
	return NULL;
}

/** \brief Return true if the kad_bstrap_t may stop, false otherwise
 */
bool	kad_bstrap_t::may_stop()	const throw()
{
	kad_kbucket_t *	kad_kbucket	= kad_peer->get_kbucket();
	delay_t		bstrap_age	= date_t::present() - start_date;
	// get the whole kad_kbucket in a kad_addr_arr_t
	kad_addr_arr_t	kad_addr_arr	= kad_kbucket->get_noldest_addr(std::numeric_limits<size_t>::max());
	size_t		nb_any		= kad_addr_arr.size();
	// count the number of public ip_addr_t present in the kad_kbucket_t
	size_t	nb_public	= 0;
	for(size_t i = 0; i < kad_addr_arr.size(); i++){
		const kad_addr_t & kad_addr	= kad_addr_arr[i];
		if( kad_addr.oaddr().ipaddr().is_public() )	nb_public++;
	}

	// handle the stopping criteria according to the age of the kad_bstrap_t
	if( bstrap_age <= profile.stop_multpub_delay() ){
		// if in stage multpub
		if( nb_public >= profile.stop_multpub_npeer() )	return true;
		else						return false;
	}else if( bstrap_age <= profile.stop_multany_delay() ){
		// if in stage multpub
		if( nb_any >= profile.stop_multany_npeer() )	return true;
		else						return false;
	}else if( bstrap_age <= profile.stop_singany_delay() ){
		// if in stage multpub
		if( nb_any >= profile.stop_singany_npeer() )	return true;
		else						return false;
	}

	// if this point is reached, the kad_bstrap_t totally give up
	return true;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     kad_bstrap_src callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback notified by \ref kad_bstrap_src_t when to notify an ipport_addr_t
 */
bool kad_bstrap_t::neoip_kad_bstrap_src_cb(void *cb_userptr, kad_bstrap_src_t &cb_kad_bstrap_src
							, const kad_addr_t &kad_addr)	throw()
{
	kad_bstrap_cand_t *	bstrap_cand;
	// log to debug
	KLOG_ERR("enter notified kad_addr=" << kad_addr);
	
	// if this kad_addr_t is from the local kad_peer_t, discard it
	if( kad_addr.get_peerid() == kad_peer->local_peerid() ){
		// put this kad_addr_t in the negcache - as it is useless to retry it
		bstrap_src->push_negcache(kad_addr, delay_t());
		// ask for for to the kad_bstrap_src_t
		bstrap_src->get_more();
		return true;
	}
	
	// try to find a kad_bstrap_cand_t using the notified kad_addr_t
	bstrap_cand	= cand_by_kad_addr(kad_addr);
	// if this kad_addr_t is already used by a kad_bstrap_cand_t, discard it and ask for more
	if( bstrap_cand ){
		// put this kad_addr_t in the negcache - as it is useless to retry it for a while
		bstrap_src->push_negcache(kad_addr, delay_t());
		// ask for for to the kad_bstrap_src_t
		bstrap_src->get_more();
		return true;
	}
	
	// log to debug
	KLOG_ERR("launch new candidate on " << kad_addr);
	// launch a candidate on this remote_addr
	kad_err_t	kad_err;
	bstrap_cand	= nipmem_new kad_bstrap_cand_t(this);
	kad_err		= bstrap_cand->start(kad_addr, this, NULL);
	if( kad_err.failed() )		nipmem_zdelete	bstrap_cand;

	// if the current number of candidates is not the maximum, ask for more
	if( cand_db.size() < profile.cand_nb_concurrent() )	bstrap_src->get_more();

	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      kad_bstrap_cand_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_bstrap_cand_t is completed
 */
bool kad_bstrap_t::neoip_kad_bstrap_cand_cb(void *cb_userptr, kad_bstrap_cand_t &cb_kad_bstrap
								, const kad_err_t &kad_err)	throw()
{
	kad_bstrap_cand_t *	cand		= &cb_kad_bstrap;
	kad_addr_t		remote_addr	= cand->get_remote_addr();
	kad_kbucket_t *		kad_kbucket	= kad_peer->get_kbucket();
	// log to debug
	KLOG_DBG("enter remote_addr=" << remote_addr << " kad_err=" << kad_err);
	// delete the candidate
	nipmem_zdelete	cand;
	
	// put this in the bstrap_src's negcache whatever the result
	// - if it failed, put it only for the default delay
	// - if it succeed, put it for an infinite amount of time
	if( kad_err.failed() )	bstrap_src->push_negcache(remote_addr, delay_t());
	else			bstrap_src->push_negcache(remote_addr, delay_t::INFINITE);

	// launch the refreshing of "all k-buckets further away than its closest neighbor."
	// - TODO unclear WHEN this should be done
	// - currently i do it after each successfull btrap_cand_t, this help populate the kbucket
	// - thus helps the stopping criteria.
	// - TODO maybe an issue with the fact that the stopping criteria is tested immediatly
	//   while the refresh_post_bstrap() is async. 
	if( kad_err.succeed() )	kad_kbucket->refresh_post_bstrap();

	// if the current number of candidates is not the maximum, ask for more
	if( cand_db.size() < profile.cand_nb_concurrent() )	bstrap_src->get_more();

	// if the kad_strap_t may_stop() now, notify the caller
	if( may_stop() )	return notify_callback();

	// return dontkeep - as the notifier has been deleted
	return false;	
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
bool 	kad_bstrap_t::notify_callback()	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_bstrap_cb(userptr, *this);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - the caller is supposed to delete the kad_bstrap_t 
	DBG_ASSERT( tokeep == false );
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END;







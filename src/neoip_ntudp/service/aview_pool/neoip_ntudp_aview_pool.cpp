/*! \file
    \brief Definition of the ntudp_aview_pool_t

\par Description of how the pool is scheduled
- When a caller pickup an client result, it is replaced.
- if a client fails during the itor of the npos_saddrecho_t
  - this means the destination is unreachable
  - but still the npos_saddrecho_t is needed
  => relaunch it on another server
- if a client fails during the full of the npos_saddrecho_t (timedout or remote_addr going offline)
  - this means the destination is unreachable.
  - but that no caller picked up the result before it fails, so it is considered useless load
    to relaunch it.
- When starting the pool, it is possible to initialize the pool with a number of npos_saddrecho.
  - it may be an usefull optimization as the 'boot' is likely a moment where connection are made.

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_aview_pool.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
#include "neoip_ntudp_npos_saddrecho.hpp"
#include "neoip_ntudp_npos_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_aview_pool_t::ntudp_aview_pool_t()		throw()
{
	// zero some fields
	ntudp_peer	= NULL;
	new_aview_hook	= event_hook_t(1);	
}

/** \brief destructor
 */
ntudp_aview_pool_t::~ntudp_aview_pool_t()		throw()
{
	// free all saddrecho_db
	while( !saddrecho_db.empty() ){
		std::set<ntudp_npos_saddrecho_t *>::iterator	iter = saddrecho_db.begin();
		// delete the object itself
		nipmem_delete	*iter;
		// remove it from the saddrecho_db
		saddrecho_db.erase(iter);
	}
	
	// unsubscribe to the pserver_pool_t if needed
	if( ntudp_peer ){
		ntudp_pserver_pool_t *	pserver_pool = ntudp_peer->pserver_pool();	
		pserver_pool->new_unknown_unsubscribe(this, NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      start() function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
ntudp_err_t	ntudp_aview_pool_t::start(ntudp_peer_t *ntudp_peer, size_t nb_initial)	throw()
{
	// copy some parameter
	this->ntudp_peer	= ntudp_peer;
	this->nb_needed_aview	= nb_initial;
	// subscribe to the pserver_pool_t
	ntudp_pserver_pool_t *	pserver_pool = ntudp_peer->pserver_pool();	
	pserver_pool->new_unknown_subscribe(this, NULL);
	
	// create all the needed saddrecho
	create_needed_saddrecho();

	// return no error
	return ntudp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the number of ipport_aview_t available
 */
size_t	ntudp_aview_pool_t::size()	const throw()
{
	std::set<ntudp_npos_saddrecho_t *>::const_iterator	iter;
	size_t	nb_avail_pview = 0;
	// scan the whole database
	for( iter = saddrecho_db.begin(); iter != saddrecho_db.end(); iter++ ){
		const ntudp_npos_saddrecho_t *	npos_saddrecho = *iter;
		// if this npos_saddrecho_t is still in itor phase, skip it
		if( npos_saddrecho->phase_is_itor() )	continue;
		// increase the number of available ipport_aview_t
		nb_avail_pview++;
	}
	// return the number of available ipport_aview_t
	return nb_avail_pview;
}

/** \brief Return one ntudp_npos_saddrecho_t from the pool
 * 
 * - if no ntudp_npos_saddrecho_t is available when this function is called, the 
 *   nb_needed_aview is increased and NULL if returned
 * - up to the caller to properly handle the stolen ntudp_npos_addrecho_t
 *   - e.g. set a callback
 * - the stolen ntudp_npos_saddrecho_t * MUST be freed by nipmem_free by the caller
 */
ntudp_npos_saddrecho_t*	ntudp_aview_pool_t::steal_one_saddrecho()	throw()
{
	std::set<ntudp_npos_saddrecho_t *>::iterator	iter;
	ntudp_npos_saddrecho_t *			npos_saddrecho = NULL;
	// if no ntudp_npos_saddrecho_t is available, increase nb_needed_aview
	if( size() == 0 ){
		nb_needed_aview++;
		// create all the needed saddrecho
		create_needed_saddrecho();
		// return NULL
		return NULL;
	}
	
	// scan the whole database to find one in full phase
	for( iter = saddrecho_db.begin(); iter != saddrecho_db.end(); iter++ ){
		// init cli_saddecho
		npos_saddrecho	= *iter;
		// if this npos_saddrecho_t is still in itor phase, skip it
		if( npos_saddrecho->phase_is_itor() )	continue;
		// if one full is found, exit the loop
		break;
	}

	// some aliasing to improve the readability
	DBG_ASSERT( iter != saddrecho_db.end() );
	// the npos_saddrecho is now 'consumed, remove it from the database but DONT delete it
	saddrecho_db.erase(iter);

	// create all the needed saddrecho
	create_needed_saddrecho();

	// ensure the npos_saddrecho will nomore notify here
	// - this helps detecting caller which forgot to setup the callback after the stealth
	npos_saddrecho->set_callback(NULL, NULL);
	
	// return the result now
	return npos_saddrecho;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           saddrecho creation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Try to create all the needed saddrecho
 */
void	ntudp_aview_pool_t::create_needed_saddrecho()	throw()
{
	size_t	nb_to_create	= nb_needed_aview - saddrecho_db.size();
	//  start all the initial discovery
	for(size_t i = 0; i < nb_to_create; i++)	create_one_saddrecho();
}


/** \brief Create a new ntudp_npos_saddrecho_t
 * 
 * @return true on error, false otherwise
 */
bool	ntudp_aview_pool_t::create_one_saddrecho()	throw()
{
	ntudp_pserver_pool_t *		pserver_pool = ntudp_peer->pserver_pool();
	ntudp_npos_saddrecho_t *	npos_saddrecho;
	ntudp_err_t			ntudp_err;
	ipport_addr_t			remote_addr;
	
	// log to debug
	KLOG_DBG("enter");
	
	// get the remote_addr from the pserver_pool
	// - first try to get a reachable address
	// - if not found, try to get an unknown one
	// - if still not found, ask for more unknown address and return an error
	if( pserver_pool->reach_size() > 0 ){
		// if there are some reachable address, pick one at random
		remote_addr	= pserver_pool->reach_at(rand() % pserver_pool->reach_size());
	}else if( pserver_pool->unknown_size() > 0 ){
		// if there are some unknown address, pick one at random
		remote_addr	= pserver_pool->unknown_at(rand() % pserver_pool->unknown_size());
	}else{	// ask for more unknown address to the pserver_pool_t
		pserver_pool->get_more_unknown();
		// return an error
		return true;
	}
	
	// create a new npos_saddrecho toward the remote_addr
	npos_saddrecho	= nipmem_new ntudp_npos_saddrecho_t();
	ntudp_err	= npos_saddrecho->start(remote_addr, ntudp_peer->pserver_pool()
								, this, NULL);
	// handle the error case
	if( ntudp_err.failed() ){
		nipmem_delete npos_saddrecho;
		return true;
	}
	// put it in the database
	bool	succeed	= saddrecho_db.insert(npos_saddrecho).second;
	DBG_ASSERT( succeed );
	// return no error
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ntudp_pserver_pool_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a event_hook_t level is notified
 * @return a 'tokeep' for the event_hook_t object
 */
bool	ntudp_aview_pool_t::neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// create all the needed saddrecho
	create_needed_saddrecho();
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ntudp_npos_saddrecho_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by ntudp_npos_saddrecho_t when the result is known
 */
bool ntudp_aview_pool_t::neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
					, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
					, const ntudp_npos_event_t &ntudp_npos_event)	throw() 
{
	// sanity check - the event MUST be npos_saddrecho_ok
	DBG_ASSERT( ntudp_npos_event.is_npos_saddrecho_ok() );
	// sanity check - the notifier MUST be in the database
	DBG_ASSERT( saddrecho_db.find(&cb_ntudp_npos_saddrecho) != saddrecho_db.end() );
	// display the result
	KLOG_DBG("ntudp_npos_saddrecho notified ntudp_npos_event=" << ntudp_npos_event);

	// if an error occured, handle it
	if( ntudp_npos_event.is_fatal() ){
		ntudp_npos_saddrecho_t *	npos_saddrecho	= &cb_ntudp_npos_saddrecho;
		bool				was_in_itor	= npos_saddrecho->phase_is_itor();
		// delete the object and remove it from the database
		saddrecho_db.erase(npos_saddrecho);
		nipmem_delete npos_saddrecho;
		// handle the event depending on the npos_saddrecho itor/full phase
		if( was_in_itor ){
			//the error occured in the itor phase, replace it 
			create_needed_saddrecho();
		}else{
			// decrease the nb_needed_aview as it failed during the full phase
			DBG_ASSERT( nb_needed_aview >= 1 );
			nb_needed_aview--;
		}
		// return 'dontkeep'
		return false;
	}
	
	// if the ntdup_npos_event is completed and true, notify the local event_hook
	if( ntudp_npos_event.is_completed() ){
		// sanity check - ntudp_npos_saddrecho_t never notify a completed with false
		DBG_ASSERT( ntudp_npos_event.get_completed_result() == true );
		// notify all the subscribers of the new_aview_hook
		new_aview_hook.notify(0);
	}

	// return 'tokeep'
	return true;
}


NEOIP_NAMESPACE_END




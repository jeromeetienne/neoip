/*! \file
    \brief Definition of the ntudp_npos_eval_t

\par Algorithm
- The scheduling is done to have a tunable speed for the tradeoff 'complete as 
  fast as possible' vs 'reduce the network load'
- If a client of a given type is in progress toward a reachable pserver address, no
  other client of this type will be launched.
  - as it is supposed to succeed with a high probability
  - it is possible to run several clients on a given reachable pserver address 
- Its launches as many client as possible on unknown addresses but only one
  per unknown address.
  - the limit being the number of unknown address in the ntudp_pserver_pool_t
    AND the constant profile.max_concurrent_unknown()
  - thus it find a reachable address as a tunable speed, depending on profile.max_concurrent_unknown()
- the type of client running on unknown address is balanced over all possible types
  - thus if the unknown address happens to be reachable, more client will be completed.
- As soon as a client notify a sucessfull completion, all the clients of the 
  same type are deleted.


\par Implementation Notes
- all revolves around the create_needed_client()
  - this function create the needed clients and notify completion if needed
  - thus it is callable easily from a lot of place without having complex states.
    this simplify the code.
  - this function is called in the start(), in the notification from ntudp_pserver_pool_t
    and at the completion of a client
  - this function is implemented as a loop over the launch of the 4 types of client
    - the loop exists when all launcher failed to launch a client
    - having all type of client in the loop allow to "type of client running on unknown
      address is balanced over all possible types"
- this module used some n^3 functions :) to get the get_least_used
  - it allows nices scheduling to optimize the network load
  - but it is no issue as this modules is not at all in the critical path. 
    - to be run once every 5min would be quite a lot already
  - and that n is rather small

\par How a given client type is handled
- in order to keep the code simple, all type of client are handled similarly
- Creation aka in launch_*() function
  - if the test is already done, dont launch
  - if it cant be done now, dont launch
    - e.g. natsym() requires natted() to be completed
  - if some instance of this client are inprogress on a reachable address, dont launch
    - it is supposed to succeed with a high probability so no need to flood with more
  - if some reachable address are available, launch the client on the least used reachable address
    - NOTE: here available means available in the pserver_pool AND not conflicting with 
      other requirement 
      - such as natsym() MUST NOT use the same pserver_addr as natted()
      - and with no other client of this type being inprogress on it
  - if some unknown address are available AND less than profile.max_concurrent_unknown() clients
    are running on unknown address, launch the client on the least used unknown address
    - NOTE: here available means the same as for reachable
  - if less than profile.max_concurrent_unknown() clients are running on unknown addresses, ask for more
    unknown address to the pserver_pool
- Completion aka how to handle the callback notification from this client
  - if the client sucessfully complete:
    -# update the suitable field in current_result
    -# delete all clients of this type
  - if it failed to complete:
    -# delete this particular client
  - in both case, call create_needed_client()


*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_eval.hpp"
#include "neoip_ntudp_npos_eval_count_db.hpp"
#include "neoip_ntudp_npos_event.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
#include "neoip_ntudp_npos_inetreach.hpp"
#include "neoip_ntudp_npos_saddrecho.hpp"
#include "neoip_ntudp_npos_natlback.hpp"
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
ntudp_npos_eval_t::ntudp_npos_eval_t()		throw()
{
	// zero some field
	pserver_pool	= NULL;
	npos_server	= NULL;
}

/** \brief Desstructor
 */
ntudp_npos_eval_t::~ntudp_npos_eval_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");

	// unsubscribe to the pserver_pool_t if needed
	if( pserver_pool )	pserver_pool->new_unknown_unsubscribe(this, NULL);

	// close all the pending inetreach
	while( !inetreach_db.empty()  ){
		std::set<ntudp_npos_inetreach_t *>::iterator	iter = inetreach_db.begin();
		nipmem_delete *iter;
		inetreach_db.erase(iter);
	}
	// close all the pending natted
	while( !natted_db.empty()  ){
		std::set<ntudp_npos_saddrecho_t *>::iterator	iter = natted_db.begin();
		nipmem_delete *iter;
		natted_db.erase(iter);
	}
	// close all the pending natsym
	while( !natsym_db.empty()  ){
		std::set<ntudp_npos_saddrecho_t *>::iterator	iter = natsym_db.begin();
		nipmem_delete *iter;
		natsym_db.erase(iter);
	}
	// close all the pending natlback
	while( !natlback_db.empty()  ){
		std::set<ntudp_npos_natlback_t *>::iterator	iter = natlback_db.begin();
		nipmem_delete *iter;
		natlback_db.erase(iter);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_npos_eval_t &	ntudp_npos_eval_t::set_profile(const ntudp_npos_eval_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
ntudp_err_t	ntudp_npos_eval_t::start(ntudp_pserver_pool_t *pserver_pool
					, ntudp_npos_server_t *npos_server
					, const ipport_addr_t &listen_addr_pview_cfg
					, ntudp_npos_eval_cb_t *callback, void *userptr)	throw()
{
	// sanity check - the ntudp_npos_eval_t MUST NOT be already start()ed
	DBG_ASSERT( this->pserver_pool == NULL );
	
	// copy the parameters
	this->pserver_pool	= pserver_pool;
	this->npos_server	= npos_server;
	this->listen_addr_pview_cfg= listen_addr_pview_cfg;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// launch the global timeout
	expire_timeout.start(profile.expire_delay(), this, NULL);
	// subscribe to the pserver_pool_t
	pserver_pool->new_unknown_subscribe(this, NULL);

	// create all the needed clients
	create_needed_client();	

	// return no error
	return ntudp_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool	ntudp_npos_eval_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// as the expire_timeout just expired, notify a null ntudp_npos_res_t	
	return notify_callback(ntudp_npos_res_t());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ntudp_pserver_pool_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a event_hook_t level is notified
 * 
 * @return a 'tokeep' for the event_hook_t object
 */
bool	ntudp_npos_eval_t::neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// create all the needed clients
	create_needed_client();	
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Create all needed clients
 * 
 * - WARNING: this function MAY delete the ntudp_npos_eval_t
 * 
 * @return a tokeep for the ntudp_npos_eval_t object
 */
bool	ntudp_npos_eval_t::create_needed_client()	throw()
{
	// if the current_res is completed, notify it to the caller
	if( current_res.completed() )	return notify_callback(current_res);
	
	// launch more clients
	bool	launched_one;	
	do {
		launched_one	= false;
		launched_one	|= launch_inetreach();
		launched_one	|= launch_natted();
		launched_one	|= launch_natsym();
		launched_one	|= launch_natlback();
		// loop until not even one launch_* function succeed
	} while( launched_one );
	
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Count the number of client (of any type) running on pserver address of 
 *         unknown reachability.
 * 
 * - it is used to see if the profile.max_concurrent_unknown() limit is reached or not
 * 
 * @return the number of client on unknown
 * 
 */
size_t	ntudp_npos_eval_t::count_nb_client_on_unknown()	const throw()
{
	count_db_t	count_db(this);
	// count all the clients
	count_db.count_all_inetreach();
	count_db.count_all_natted();
	count_db.count_all_natsym();
	count_db.count_all_natlback();
	// keep only the addresses of unknown state
	count_db.intersect_unknown();
	// return the sum
	return count_db.get_sum_count();
}

/** \brief return the least used reachable pserver_addr
 * 
 * @return the 'best' ipport_addr_t, may be null if none has been found
 */
ipport_addr_t	ntudp_npos_eval_t::get_least_used_reach(const count_db_t &excluded_db)	const throw()
{
	count_db_t	count_db(this);
	// make sure all the unknown address from pserver_pool are present in the count_db_t
	count_db.union_reach();
	// count all the clients
	count_db.count_all_inetreach();
	count_db.count_all_natted();
	count_db.count_all_natsym();
	count_db.count_all_natlback();
	// exclude the address from the excluded_db
	count_db.exclude(excluded_db);
	// keep only the addresses of reachable state
	count_db.intersect_reach();	
	// return the least used
	return count_db.get_least_counted();
}

/** \brief return the least used unknown pserver_addr
 * 
 * @return the 'best' ipport_addr_t, may be null if none has been found
 */
ipport_addr_t	ntudp_npos_eval_t::get_least_used_unknown(const count_db_t &excluded_db) const throw()
{
	count_db_t	count_db(this);

	// make sure all the unknown address from pserver_pool are present in the count_db_t
	count_db.union_unknown();
		
	// count all the clients
	count_db.count_all_inetreach();
	count_db.count_all_natted();
	count_db.count_all_natsym();
	count_db.count_all_natlback();

	// exclude the address from the exclude_db
	count_db.exclude(excluded_db);
	// keep only the addresses of unknown state
	count_db.intersect_unknown();	

	// return the least used
	return count_db.get_least_counted();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      inetreach stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief try to launch ntudp_npos_inetreach_t client
 * 
 * @return true if one is launched, false
 */
bool	ntudp_npos_eval_t::launch_inetreach()	throw()
{
	std::set<ntudp_npos_inetreach_t *>::iterator	iter;
	ipport_addr_t	pserver_addr;
	// log to debug
	KLOG_DBG("enter");

	// if the client has already been completed, no need to launch a new one
	if( current_res.inetreach_present() )		return false;

	// if the listen_addr_pview_cfg is null, this peer will never be inetreach
	// => set inetreach to false and dont launch any client
	// - NOTE: this may change in the future e.g. in case of upnp support
	if( listen_addr_pview_cfg.is_null() ){
		current_res.inetreach( false );
		return false;
	}
	
	// sanity check - the listen_addr_pview_cfg MUST qualify a port
	DBG_ASSERT( listen_addr_pview_cfg.get_port() != 0 );
	// if the listen_addr_pview_cfg dont qualify the ip_addr, try to get it from natted() result
	ipport_addr_t	probe_addr	= listen_addr_pview_cfg;
	if( probe_addr.ipaddr().is_any() ){
		// if natted() is not yet completed, dont launch
		if( !current_res.natted_present() )	return false;
		// if natted() is completed, get the public view of the natted_aview
		// - NOTE: this assume the local peer MUST NOT have multiple public view
		//   ip address. this may not be true in case of large nat realm in which
		//   multiple nat box provide multiple ip address for a single natted host.
		probe_addr.set_ipaddr( natted_aview.pview().ipaddr() );
	}
	// sanity check - here the probe_addr MUST be fully qualified
	DBG_ASSERT( probe_addr.is_fully_qualified() );
	

	// if at least one instance of this client is on a reachable address, dont launch more
	if( !count_db_t(this).count_all_inetreach().intersect_reach().empty() )	return false;

	// build the exclude_db with all instances for this type of client
	// - to be sure not to launch several client on a given type on a given address
	count_db_t	exclude_db	= count_db_t(this).count_all_inetreach();

	// if some reachable address are available, set pserver_addr with the least used reachable address
	if( pserver_pool->reach_size() > 0 )
		pserver_addr	= get_least_used_reach(exclude_db);
	// count the number of client running on pserver address of unknown reachability
	size_t	nb_unknown = count_nb_client_on_unknown();

	// if no reachable address are available AND that current number of client on unknown 
	// address is less than the maximum AND the pserver_pool contains some unknown address
	// set pserver_addr with the least used reachable address
	if( pserver_addr.is_null() && pserver_pool->unknown_size() && nb_unknown < profile.max_concurrent_unknown())
		pserver_addr	= get_least_used_unknown( exclude_db );

	// if a suitable pserver_addr has been found, launch the client on it
	if( !pserver_addr.is_null() ){
		ntudp_npos_inetreach_t *inetreach;
		ntudp_err_t		ntudp_err;
		// log to debug
		KLOG_DBG("launch a inetreach client to discover if " << probe_addr
						<< " is reachable using pserver " << pserver_addr);
		// launch the inetreach test client	
		inetreach	= nipmem_new ntudp_npos_inetreach_t();
		ntudp_err	= inetreach->set_profile(profile.cli()).start(pserver_addr
						, probe_addr, npos_server, pserver_pool, this, NULL);
		// if it failed, ask for more unknown address and return false
		if( ntudp_err.failed() ){
			nipmem_delete	inetreach;
			pserver_pool->get_more_unknown();
			return false;
		}
		// add this inetreach to the database
		inetreach_db.insert(inetreach);
		// return true to show that one client has been launched
		return true;
	}

	// NOTE: if this point is reached, no suitable pserver address have found
	DBG_ASSERT( pserver_addr.is_null() );

	// if the number of client on unknown address is less than the max, ask for more unknown address
	if( nb_unknown < profile.max_concurrent_unknown() )	pserver_pool->get_more_unknown();

	// return 'noclientlaunched'
	return false;
}

/** \brief callback notified by ntudp_npos_inetreach_t when the result is known.
 */
bool ntudp_npos_eval_t::neoip_ntudp_npos_inetreach_event_cb(void *cb_userptr
					, ntudp_npos_inetreach_t &cb_ntudp_npos_inetreach
					, const ntudp_npos_event_t &npos_event)	throw() 
{
	ntudp_npos_inetreach_t *inetreach	= &cb_ntudp_npos_inetreach;
	// sanity check - the event MUST be cli_inetreach_ok
	DBG_ASSERT( npos_event.is_npos_inetreach_ok() );
	// sanity check - the current_res MUST NOT have inetreach already present
	DBG_ASSERT( current_res.inetreach_present() == false );
	// display the result
	KLOG_DBG("ntudp_npos_inetreach notified ntudp_npos_event=" << npos_event);

	// if the event is completed, copy the result in the current_res
	if( npos_event.is_completed() ){
		// set the current ntudp_npos_res_t with the notified result
		current_res.inetreach( npos_event.get_completed_result() );
		// close all the pending inetreach
		while( !inetreach_db.empty()  ){
			std::set<ntudp_npos_inetreach_t *>::iterator	iter = inetreach_db.begin();
			nipmem_delete *iter;
			inetreach_db.erase(iter);
		}
	} else { // if an error occured delete the object and remove it from the database
		DBG_ASSERT( npos_event.is_fatal() );
		DBG_ASSERT( inetreach_db.find(inetreach) != inetreach_db.end() );
		nipmem_delete	inetreach;
		inetreach_db.erase(inetreach);
	}
	
	// call the main function either to relaunch other clients or to notify completion
	create_needed_client();

	// return 'dontkeep' - as in anycase the client as been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//               ntudp_npos_saddrecho_t callback dispatcher
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by ntudp_npos_saddrecho_t when the result is known.
 * 
 * - it is used to dispatch the event to natted or to natsym
 *   - it goes to natted if the current_res.natted_present == false
 *   - else it goes to natsym
 * - this implies to close all the natted as soon as one is completed
 */
bool ntudp_npos_eval_t::neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
					, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
					, const ntudp_npos_event_t &npos_event)	throw() 
{
	// display the result
	KLOG_DBG("ntudp_npos_saddrecho notified ntudp_npos_event=" << npos_event);
	// sanity check - the event MUST be cli_saddrecho_ok
	DBG_ASSERT( npos_event.is_npos_saddrecho_ok() );

	// if natted has not been completed, forward the event to natted
	if( current_res.natted_present() == false )
		return neoip_ntudp_npos_natted_event_cb(cb_userptr, cb_ntudp_npos_saddrecho, npos_event);

	// else forward it to natsym
	return neoip_ntudp_npos_natsym_event_cb(cb_userptr, cb_ntudp_npos_saddrecho, npos_event);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      natted stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief try to launch ntudp_npos_saddrecho_t client for natted
 * 
 * @return true if one is launched, false
 */
bool	ntudp_npos_eval_t::launch_natted()	throw()
{
	std::set<ntudp_npos_saddrecho_t *>::iterator	iter;
	ipport_addr_t	pserver_addr;

	// if the client has already been completed, no need to launch a new one
	if( current_res.natted_present() )		return false;

	// if at least one instance of this client is on a reachable address, dont launch more
	if( !count_db_t(this).count_all_natted().intersect_reach().empty() )	return false;

	// build the exclude_db with all instances for this type of client
	// - to be sure not to launch several client on a given type on a given address
	count_db_t	exclude_db	= count_db_t(this).count_all_natted();

	// if some reachable address are available, set pserver_addr with the least used reachable address
	if( pserver_pool->reach_size() > 0 )
		pserver_addr	= get_least_used_reach(exclude_db);

	// count the number of client running on pserver address of unknown reachability
	size_t	nb_unknown = count_nb_client_on_unknown();

	// if no reachable address are available AND that current number of client on unknown 
	// address is less than the maximum AND the pserver_pool contains some unknown address
	// set pserver_addr with the least used reachable address
	if( pserver_addr.is_null() && pserver_pool->unknown_size() && nb_unknown < profile.max_concurrent_unknown())
		pserver_addr	= get_least_used_unknown(exclude_db);

	// if a suitable pserver_addr has been found, launch the client on it
	if( !pserver_addr.is_null() ){
		ntudp_npos_saddrecho_t*	saddrecho;
		ntudp_err_t		ntudp_err;
		// log to debug
		KLOG_DBG("launch a saddrecho for natted client using pserver " << pserver_addr);
		// launch the saddrecho client for natted
		saddrecho	= nipmem_new ntudp_npos_saddrecho_t();
		ntudp_err	= saddrecho->set_profile(profile.cli()).start(pserver_addr
									, pserver_pool, this, NULL);
		// if it failed, ask for more unknown address and return false
		if( ntudp_err.failed() ){
			nipmem_delete	saddrecho;
			pserver_pool->get_more_unknown();
			return false;
		}
		// add this saddrecho to the natted database
		natted_db.insert(saddrecho);
		// return true to show that one client has been launched
		return true;
	}

	// NOTE: if this point is reached, no suitable pserver address have found
	DBG_ASSERT( pserver_addr.is_null() );

	// if the number of client on unknown address is less than the max, ask for more unknown address
	if( nb_unknown < profile.max_concurrent_unknown() )	pserver_pool->get_more_unknown();

	// return 'noclientlaunched'
	return false;
}

/** \brief callback notified by ntudp_npos_saddrecho_t for natted when the result is known.
 * 
 * - this function is not called directly but by the neoip_ntudp_npos_saddrecho_event_cb dispatched
 */
bool ntudp_npos_eval_t::neoip_ntudp_npos_natted_event_cb(void *cb_userptr
					, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
					, const ntudp_npos_event_t &npos_event)	throw() 
{
	ntudp_npos_saddrecho_t *saddrecho	= &cb_ntudp_npos_saddrecho;
	// sanity check - the event MUST be cli_saddrecho_ok
	DBG_ASSERT( npos_event.is_npos_saddrecho_ok() );
	// sanity check - the current_res MUST NOT have natted already present
	DBG_ASSERT( current_res.natted_present() == false );
	// display the result
	KLOG_DBG("ntudp_npos_saddrecho for natted() notified ntudp_npos_event=" << npos_event);

	// if the event is completed, copy the result in the current_res
	if( npos_event.is_completed() ){
		// get the local/public view of the saddrecho for natsym 
		natted_aview		= saddrecho->local_addr_aview();
		// copy the pserver address of this client for natsym later
		natted_pserver_addr	= saddrecho->get_pserver_addr();
		// set the local_res.local_ipaddr_lview/pview with the result of this test client
		current_res.local_ipaddr_lview( natted_aview.lview().ipaddr() );
		current_res.local_ipaddr_pview( natted_aview.pview().ipaddr() );
		// if is natted IIF the local_view is different from the public_view
		if( natted_aview.is_natted() )	current_res.natted(true);
		else				current_res.natted(false);
		// close all the pending natted
		while( !natted_db.empty()  ){
			std::set<ntudp_npos_saddrecho_t *>::iterator	iter = natted_db.begin();
			nipmem_delete *iter;
			natted_db.erase(iter);
		}
	} else { // if an error occured delete the object and remove it from the database
		DBG_ASSERT( npos_event.is_fatal() );
		DBG_ASSERT( natted_db.find(saddrecho) != natted_db.end() );
		nipmem_delete	saddrecho;
		natted_db.erase(saddrecho);
	}
	
	// call the main function either to relaunch other clients or to notify completion
	create_needed_client();

	// return 'dontkeep' - as in anycase the client as been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      natsym stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief try to launch ntudp_npos_saddrecho_t client for natsym
 * 
 * @return true if one is launched, false
 */
bool	ntudp_npos_eval_t::launch_natsym()	throw()
{
	std::set<ntudp_npos_saddrecho_t *>::iterator	iter;
	ipport_addr_t	pserver_addr;

	// if the natted test has not yet been completed, doing the natsym is impossible
	if( !current_res.natted_present() )		return false;

	// if the client has already been completed, no need to launch a new one
	if( current_res.natsym_present() )		return false;

	// if at least one instance of this client is on a reachable address, dont launch more
	if( !count_db_t(this).count_all_natsym().intersect_reach().empty() )	return false;

	// build the exclude_db with all instances for this type of client
	// - include the natted_pserver_addr to exclude it 
	// - to be sure not to launch several client on a given type on a given address
	count_db_t exclude_db= count_db_t(this).count_all_natsym().count_one_address(natted_pserver_addr);

	// if some reachable address are available, set pserver_addr with the least used reachable address
	if( pserver_pool->reach_size() > 0 )
		pserver_addr	= get_least_used_reach(exclude_db);

	// count the number of client running on pserver address of unknown reachability
	size_t	nb_unknown = count_nb_client_on_unknown();

	// if no reachable address are available AND that current number of client on unknown 
	// address is less than the maximum AND the pserver_pool contains some unknown address
	// set pserver_addr with the least used reachable address
	// - excluse the natted_pserver_addr not to use the same as natted()
	if( pserver_addr.is_null() && pserver_pool->unknown_size() && nb_unknown < profile.max_concurrent_unknown())
		pserver_addr	= get_least_used_unknown(exclude_db);

	// if a suitable pserver_addr has been found, launch the client on it
	if( !pserver_addr.is_null() ){
		ntudp_npos_saddrecho_t*	saddrecho;
		ntudp_err_t		ntudp_err;
		// log to debug
		KLOG_DBG("launch a saddrecho for natsym client using pserver " << pserver_addr
						<< " and local_addr=" << natted_aview.lview());
		// sanity check - the pserver_addr MUST NOT be natted_pserver_addr
		DBG_ASSERT( pserver_addr != natted_pserver_addr );
		// launch the saddrecho client for natsym
		saddrecho	= nipmem_new ntudp_npos_saddrecho_t();
		// set the local_addr of this saddrecho to be the same as natted()
		ntudp_err	= saddrecho->set_local_addr(natted_aview.lview());
		if( ntudp_err.succeed() ){
			ntudp_err = saddrecho->set_profile(profile.cli()).start(pserver_addr
									, pserver_pool, this, NULL);
		}
		// if it failed, ask for more unknown address and return false
		if( ntudp_err.failed() ){
			nipmem_delete	saddrecho;
			pserver_pool->get_more_unknown();
			return false;
		}
		// add this saddrecho to the natsym database
		natsym_db.insert(saddrecho);
		// return true to show that one client has been launched
		return true;
	}

	// NOTE: if this point is reached, no suitable pserver address have found
	DBG_ASSERT( pserver_addr.is_null() );

	// if the number of client on unknown address is less than the max, ask for more unknown address
	if( nb_unknown < profile.max_concurrent_unknown() )	pserver_pool->get_more_unknown();

	// return 'noclientlaunched'
	return false;
}

/** \brief callback notified by ntudp_npos_saddrecho_t for natsym when the result is known.
 * 
 * - this function is not called directly but by the neoip_ntudp_npos_saddrecho_event_cb dispatched
 */
bool ntudp_npos_eval_t::neoip_ntudp_npos_natsym_event_cb(void *cb_userptr
					, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
					, const ntudp_npos_event_t &npos_event)	throw() 
{
	ntudp_npos_saddrecho_t *saddrecho	= &cb_ntudp_npos_saddrecho;
	// sanity check - the event MUST be cli_saddrecho_ok
	DBG_ASSERT( npos_event.is_npos_saddrecho_ok() );
	// sanity check - the current_res MUST NOT have natsym already present
	DBG_ASSERT( current_res.natsym_present() == false );
	// display the result
	KLOG_DBG("ntudp_npos_saddrecho for natsym() notified ntudp_npos_event=" << npos_event);

	// if the event is completed, copy the result in the current_res
	if( npos_event.is_completed() ){
		ipport_aview_t	local_aview	= saddrecho->local_addr_aview();
		// sanity check - the local_lview MUST be the same as the natted()
		DBG_ASSERT( local_aview.lview() == natted_aview.lview() );
		// sanity check - this pserver_addr MUST be different from the natted one
		DBG_ASSERT( natted_pserver_addr != saddrecho->get_pserver_addr() );
		// it is natsym IIF the local_pview of this saddrecho is != from the natted() one
		if( local_aview.pview() != natted_aview.pview() )	current_res.natsym(true);
		else							current_res.natsym(false);
		// close all the pending natsym
		while( !natsym_db.empty()  ){
			std::set<ntudp_npos_saddrecho_t *>::iterator	iter = natsym_db.begin();
			nipmem_delete *iter;
			natsym_db.erase(iter);
		}
	}else { // if an error occured delete the object and remove it from the database
		DBG_ASSERT( npos_event.is_fatal() );
		DBG_ASSERT( natsym_db.find(saddrecho) != natsym_db.end() );
		nipmem_delete	saddrecho;
		natsym_db.erase(saddrecho);
	}
	
	// call the main function either to relaunch other clients or to notify completion
	create_needed_client();

	// return 'dontkeep' - as in anycase the client as been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      natlback stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief try to launch ntudp_npos_natlback_t client
 * 
 * @return true if one is launched, false
 */
bool	ntudp_npos_eval_t::launch_natlback()	throw()
{
	std::set<ntudp_npos_natlback_t *>::iterator	iter;
	ipport_addr_t	pserver_addr;

	// if the client has already been completed, no need to launch a new one
	if( current_res.natlback_present() )		return false;

	// if at least one instance of this client is on a reachable address, dont launch more
	if( !count_db_t(this).count_all_natlback().intersect_reach().empty() )	return false;

	// build the exclude_db with all instances for this type of client
	// - to be sure not to launch several client on a given type on a given address
	count_db_t	exclude_db	= count_db_t(this).count_all_natlback();

	// if some reachable address are available, set pserver_addr with the least used reachable address
	if( pserver_pool->reach_size() > 0 )
		pserver_addr	= get_least_used_reach(exclude_db);

	// count the number of client running on pserver address of unknown reachability
	size_t	nb_unknown = count_nb_client_on_unknown();

	// if no reachable address are available AND that current number of client on unknown 
	// address is less than the maximum AND the pserver_pool contains some unknown address
	// set pserver_addr with the least used reachable address
	if( pserver_addr.is_null() && pserver_pool->unknown_size() && nb_unknown < profile.max_concurrent_unknown())
		pserver_addr	= get_least_used_unknown(exclude_db);

	// if a suitable pserver_addr has been found, launch the client on it
	if( !pserver_addr.is_null() ){
		ntudp_npos_natlback_t*	natlback;
		ntudp_err_t		ntudp_err;
		// log to debug
		KLOG_DBG("launch a natlback client using pserver " << pserver_addr);
		// setup the ntudp_cli_profile_t to have a much short expire_delay
		// - AS natlback operate only on private network
		//   - so it has with low latency and lower packet loss ratio than the internet
		// - AS natlback timingout is part of normal operation
		//   - i.e. if it timesout, the natlback is considered false, else it is
		//     considered true
		// - AS natlback operate only on local peer 
		//   - so not overloading any third party computers
		// - THEN provding a lower timeout increase the speed of detection without 
		//   damaging its accuracy   
		ntudp_npos_cli_profile_t	cli_profile	= profile.cli();
		cli_profile.cli_itor_pkt_rxmit	(delaygen_expboff_arg_t()
								.min_delay(delay_t::from_msec(500))
								.max_delay(delay_t::from_sec(3))
								.random_range(0.2)
								.timeout_delay(delay_t::from_sec(10)));
		// launch the natlback test client
		natlback	= nipmem_new ntudp_npos_natlback_t();
		ntudp_err	= natlback->set_profile(cli_profile).start(pserver_addr, pserver_pool
										, this, NULL);
		// if it failed, ask for more unknown address and return false
		if( ntudp_err.failed() ){
			nipmem_delete	natlback;
			pserver_pool->get_more_unknown();
			return false;
		}
		// add this natlback to the database
		natlback_db.insert(natlback);
		// return true to show that one client has been launched
		return true;
	}

	// NOTE: if this point is reached, no suitable pserver address have found
	DBG_ASSERT( pserver_addr.is_null() );

	// if the number of client on unknown address is less than the max, ask for more unknown address
	if( nb_unknown < profile.max_concurrent_unknown() )	pserver_pool->get_more_unknown();

	// return 'noclientlaunched'
	return false;
}

/** \brief callback notified by ntudp_npos_natlback_t when the result is known.
 */
bool ntudp_npos_eval_t::neoip_ntudp_npos_natlback_event_cb(void *cb_userptr
					, ntudp_npos_natlback_t &cb_ntudp_npos_natlback
					, const ntudp_npos_event_t &npos_event)	throw() 
{
	ntudp_npos_natlback_t *natlback	= &cb_ntudp_npos_natlback;
	// sanity check - the event MUST be cli_natlback_ok
	DBG_ASSERT( npos_event.is_npos_natlback_ok() );
	// sanity check - the current_res MUST NOT have natlback already present
	DBG_ASSERT( current_res.natlback_present() == false );
	// display the result
	KLOG_ERR("ntudp_npos_natlback notified ntudp_npos_event=" << npos_event);

	// if the event is completed, copy the result in the current_res
	if( npos_event.is_completed() ){
		// set the current ntudp_npos_res_t with the notified result
		current_res.natlback( npos_event.get_completed_result() );
		// close all the pending natlback
		while( !natlback_db.empty()  ){
			std::set<ntudp_npos_natlback_t *>::iterator	iter = natlback_db.begin();
			nipmem_delete *iter;
			natlback_db.erase(iter);
		}
	} else { // if an error occured delete the object and remove it from the database
		DBG_ASSERT( npos_event.is_fatal() );
		DBG_ASSERT( natlback_db.find(natlback) != natlback_db.end() );
		nipmem_delete	natlback;
		natlback_db.erase(natlback);
	}
	
	// call the main function either to relaunch other clients or to notify completion
	create_needed_client();

	// return 'dontkeep' - as in anycase the client as been deleted
	return false;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	ntudp_npos_eval_t::notify_callback(const ntudp_npos_res_t &ntudp_npos_res)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_npos_eval_cb(userptr, *this, ntudp_npos_res);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END



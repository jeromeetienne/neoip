/*! \file
    \brief Definition of the ntudp_itor_retry_t

- TODO
  - when i can't start a ntudp_itor_t a CNX_ESTA_TIMEDOUT is generated....
  - this is a real bad name for this event...
  - maybe to rename the event CNXESTA_FAILED would be better
    - well would be to generic name...


*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_itor_retry.hpp"
#include "neoip_ntudp_itor.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_itor_retry_t constant
const delay_t		ntudp_itor_retry_t::EXPIRE_DELAY_DFL		= delay_t::from_sec(60);
const delaygen_arg_t	ntudp_itor_retry_t::DELAYGEN_ARG_RETRY_DFL	= delaygen_expboff_arg_t()
									.min_delay(delay_t::from_sec(2))
									.max_delay(delay_t::from_sec(3*60))
									.random_range(0.2);
// end of constants definition

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_itor_retry_t::ntudp_itor_retry_t()		throw()
{
	// zero some fields
	ntudp_peer	= NULL;
	ntudp_itor	= nipmem_new ntudp_itor_t();
}

/** \brief destructor
 */
ntudp_itor_retry_t::~ntudp_itor_retry_t()		throw()
{
	// delete the ntudp_itor_t if needed
	if( ntudp_itor )	nipmem_delete ntudp_itor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      Setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the local address
 */
ntudp_itor_retry_t &	ntudp_itor_retry_t::set_local_addr(const ntudp_addr_t &local_addr)	throw()
{
	// forward it ntudp_itor_t
	ntudp_itor->set_local_addr(local_addr);
	// return the object itself
	return *this;
}

/** \brief Set the ntudp_event_t type to retry
 */
ntudp_itor_retry_t &	ntudp_itor_retry_t::set_retry_event(const ntudp_event_t::type &event_type)	throw()
{
	// sanity check - the event MUST NOT already be in the database
	DBG_ASSERT( retry_event_db.find(event_type) == retry_event_db.end() );
	// put it in the database
	retry_event_db.insert(event_type);
	// return the object itself
	return *this;
}

/** \brief Set the callback
 */
ntudp_itor_retry_t &ntudp_itor_retry_t::set_callback(ntudp_itor_retry_cb_t *callback
							, void *userptr)	throw()
{
	// copy some parameter
	this->callback		= callback;
	this->userptr		= userptr;
	// return the object itself
	return *this;
}

/** \brief Set the expiration delay
 */
ntudp_itor_retry_t &	ntudp_itor_retry_t::set_timeout(const delay_t &expire_delay)	throw()
{
	// copy some parameter
	this->expire_delay	= expire_delay;
	// return the object itself
	return *this;
}

/** \brief Set the remote address
 * 
 * - NOTE: this is binding the connection
 *   - if the local_addr is to be specified by the caller, it MUST be before this function.
 */
ntudp_err_t	ntudp_itor_retry_t::set_remote_addr(ntudp_peer_t *ntudp_peer
					, const ntudp_addr_t &remote_addr)	throw()
{
	// copy the parameter
	this->ntudp_peer	= ntudp_peer;
	// set the remote_addr in the ntudp_itor
	return ntudp_itor->set_remote_addr(ntudp_peer, remote_addr);	
}

/** \brief Start the operation
 */
ntudp_err_t	ntudp_itor_retry_t::start()		throw()
{
	// if the retry_event_db is empty, put UNKNOWN_HOST in it
	if( retry_event_db.empty() )	retry_event_db.insert(ntudp_event_t::UNKNOWN_HOST);
	
	// init delaygen for the packet rxmit
	retry_delaygen = delaygen_t(DELAYGEN_ARG_RETRY_DFL);

	// if expire_delay.is_null(), set it to the default
	if( expire_delay.is_null() )	expire_delay	= EXPIRE_DELAY_DFL;
	// start the timeout
	expire_timeout.start(expire_delay, this, NULL);

	// start the ntudp_itor_t
	return ntudp_itor->set_callback(this, NULL).start();
}


/** \brief Helper function to start a ntudp_itor_t with the common parameter
 */
ntudp_err_t	ntudp_itor_retry_t::start(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &remote_addr
				, ntudp_itor_retry_cb_t *callback, void *userptr)		throw()
{
	// set the callback
	set_callback(callback, userptr);
	// bind the socket
	ntudp_err_t	ntudp_err;
	ntudp_err	= set_remote_addr(ntudp_peer, remote_addr);
	if( ntudp_err.failed() )	return ntudp_err;
	// start the operation
	return start();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   ntudp_itor_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_itor_t when it has an event to notify
 */
bool ntudp_itor_retry_t::neoip_ntudp_itor_event_cb(void *cb_userptr, ntudp_itor_t &cb_ntudp_itor
							, const ntudp_event_t &ntudp_event)	throw()
{
	// log to debug
	KLOG_ERR("enter ntudp_event=" << ntudp_event );
	// sanity check - the ntudp_event_t MUST be is_itor_ok();
	DBG_ASSERT( ntudp_event.is_itor_ok() );
	// sanity check - the notified MUST be the local ntudp_itor
	DBG_ASSERT( ntudp_itor == &cb_ntudp_itor );

	// if this event is in the retry_event_db, launch a retry
	if( retry_event_db.find(ntudp_event.get_value()) != retry_event_db.end() ){
		ntudp_addr_t	local_addr	= ntudp_itor->local_addr();
		ntudp_addr_t	remote_addr	= ntudp_itor->remote_addr();
		ntudp_err_t	ntudp_err;
		// delete the ntudp_itor_t and mark it unused
		nipmem_delete	ntudp_itor;
		// init a new one (without starting it)
		ntudp_itor	= nipmem_new ntudp_itor_t();
		ntudp_itor->set_callback(this, NULL).set_local_addr(local_addr).set_timeout(delay_t::NEVER);
		ntudp_err	= ntudp_itor->set_remote_addr(ntudp_peer, remote_addr);
		// if the starting failed, notify a CNX_ESTA_TIMEDOUT
		if( ntudp_err.failed() ){
			std::string reason = "Failed to start ntudp_itor_t due to " + ntudp_err.to_string();
			notify_callback( ntudp_event_t::build_cnx_esta_timedout(reason) );
			return false;
		}
		// to start the timeout to retry
		// - the ntudp_itor_t will be started only then
		retry_timeout.start(retry_delaygen.pre_inc(), this, NULL);
		// return dontkeep as the ntudp_itor_t has just been deleted
		return false;
	}
	
	// if the ntudp_event_t is NOT UNKNOWN_HOST, simply forward it
	return notify_callback(ntudp_event);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main timeout_t  callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the expire_timeout expire
 */
bool ntudp_itor_retry_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// simply route to the proper timeout_t callback
	if( &expire_timeout == &cb_timeout )	return expire_timeout_cb(userptr, cb_timeout);
	if( &retry_timeout  == &cb_timeout )	return retry_timeout_cb(userptr, cb_timeout);
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return false;
}

/** \brief callback called when the expire_timeout expire
 */
bool ntudp_itor_retry_t::expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// notify a CNX_ESTA_TIMEDOUT to the caller	
	std::string reason	= "Connection establishment timedout";
	return notify_callback( ntudp_event_t::build_cnx_esta_timedout(reason) );	
}

/** \brief callback called when the retry_timeout expire
 */
bool ntudp_itor_retry_t::retry_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// start the ntudp_itor - it is already setup
	ntudp_err_t	ntudp_err;
	ntudp_err	= ntudp_itor->start();
	// if the starting failed, notify a CNX_ESTA_TIMEDOUT
	if( ntudp_err.failed() ){
		std::string reason = "Failed to start ntudp_itor_t due to " + ntudp_err.to_string();
		return notify_callback( ntudp_event_t::build_cnx_esta_timedout(reason) );
	}

	// stop the retry_timeout
	retry_timeout.stop();

	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	ntudp_itor_retry_t::notify_callback(const ntudp_event_t &ntudp_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_itor_retry_event_cb(userptr, *this, ntudp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - specific to ntudp_itor_retry_t, the object MUST be deleted during notification
	DBG_ASSERT( tokeep == false );	
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END




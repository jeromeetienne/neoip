/*! \file
    \brief Definition of the ntudp_npos_watch_t

\par Implementation notes
\ref ntudp_npos_watch_t does a \ref ntudp_npos_eval_t to keep a ntudp_npos_res_t 
uptodate.

- it notify the caller via callback in case of change
- this is intended to be use in \ref ntudp_peer_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_watch.hpp"
#include "neoip_ntudp_npos_eval.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
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
ntudp_npos_watch_t::ntudp_npos_watch_t()		throw()
{
	// zero some field
	ntudp_peer	= NULL;
	npos_eval	= NULL;
}

/** \brief Desstructor
 */
ntudp_npos_watch_t::~ntudp_npos_watch_t()		throw()
{
	// delete the npos_eval if needed
	nipmem_zdelete	npos_eval;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_npos_watch_t &	ntudp_npos_watch_t::set_profile(const ntudp_npos_watch_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}
/** \brief start the actions
 */
ntudp_err_t ntudp_npos_watch_t::start(ntudp_peer_t *ntudp_peer
			, ntudp_npos_watch_cb_t *callback, void * userptr)	throw()
{
	// copy the parameters
	this->ntudp_peer	= ntudp_peer;
	this->callback		= callback;
	this->userptr		= userptr;
	// set the delaygen from the profile
	delaygen		= delaygen_t(profile.eval_trigger());
	// start the timeout to trigger the ntudp_npos_eval_t
	timeout.start(delaygen.current(), this, NULL);
	// return no error
	return ntudp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Trigger an evaluation as soon as possible
 */
void	ntudp_npos_watch_t::trigger_eval_asap()	throw()
{
	// delete the pending ntudp_npos_eval_t if needed
	nipmem_zdelete	npos_eval;
	// trigger the timeout immediatly
	// - NOTE: in case of a delaygen.is_timedout(), this skew the delay_t
	//   - TODO is this important, is this ever timedout ?
	timeout.start(delay_t::from_sec(0), this, NULL);	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool ntudp_npos_watch_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("Start a ntudp_npos_eval_t");

	// sanity check - ntudp_npos_eval_t MUST NOT be already running
	DBG_ASSERT( npos_eval == NULL );

	// notify the expiration of the timeout to the delaygen
	delaygen.notify_expiration();
	// if the delaygen is is_timedout, notify an null ntudp_npos_res_t
	if( delaygen.is_timedout() ){
		// nullify the m_npos_res
		m_npos_res	= ntudp_npos_res_t();
		// notify the caller
		return notify_callback(m_npos_res);
	}

	// launch the npos_eval
	ntudp_err_t	ntudp_err;
	npos_eval	= nipmem_new ntudp_npos_eval_t();
	ntudp_err	= npos_eval->set_profile(profile.eval()).start(ntudp_peer->pserver_pool()
						, ntudp_peer->npos_server()
						, ntudp_peer->listen_aview_cfg().pview()
						, this, NULL);
	DBGNET_ASSERT( ntudp_err.succeed() );
	if( ntudp_err.failed() )	nipmem_zdelete	npos_eval;

	// stop this timeout
	timeout.stop();
	// return a 'tokeep'
	return true;
}

/** \brief callback notified by \ref ntudp_npos_eval_t when to notify an event
 */
bool ntudp_npos_watch_t::neoip_ntudp_npos_eval_cb(void *cb_userptr
					, ntudp_npos_eval_t &cb_ntudp_npos_eval
					, const ntudp_npos_res_t &ntudp_npos_res)	throw()
{
	// copy the result to use it after the object deletion
	ntudp_npos_res_t new_res	= ntudp_npos_res;
	// log to debug
	KLOG_ERR("enter ntudp_npos_res=" << ntudp_npos_res); 
	// sanity check - the notifying object MUST be npos_eval
	DBG_ASSERT( &cb_ntudp_npos_eval == npos_eval );
	// sanity check - the timeout MUST NOT be running
	DBG_ASSERT( !timeout.is_running() );
	// delete the npos_eval and mark it unused
	nipmem_zdelete	npos_eval;
	
	// if the new result is different from the current one, notify it
	if( !new_res.is_null() && new_res != m_npos_res ){
		// copy the new result as the current result
		m_npos_res	= new_res;
		// notify the change to the caller
		bool	tokeep = notify_callback(m_npos_res);
		if( !tokeep )	return false;
	}

	// relaunch the timeout
	timeout.start(delaygen.pre_inc(), this, NULL);
	
	// return 'dontkeep'
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
bool 	ntudp_npos_watch_t::notify_callback(const ntudp_npos_res_t &ntudp_npos_res)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_npos_watch_cb(userptr, *this, ntudp_npos_res);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



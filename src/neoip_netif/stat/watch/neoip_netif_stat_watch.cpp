/*! \file
    \brief Definition of the netif_stat_watch_t

\par Implementation notes
\ref netif_stat_watch_t periodically get the netif_stat_t and report it

*/

/* system include */
/* local include */
#include "neoip_netif_stat_watch.hpp"
#include "neoip_netif_stat_helper.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;



// definition of \ref netif_stat_watch_t constant
const delaygen_arg_t	netif_stat_watch_t::DELAYGEN_ARG_DFL	= delaygen_regular_arg_t()
									.period(delay_t::from_sec(60));	
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
netif_stat_watch_t::netif_stat_watch_t()		throw()
{
	// configure the delaygen to launch the evaluation
	delaygen	= delaygen_t(DELAYGEN_ARG_DFL);	
}

/** \brief Desstructor
 */
netif_stat_watch_t::~netif_stat_watch_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 Start() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief start the actions
 */
netif_err_t netif_stat_watch_t::start(netif_stat_watch_cb_t *callback, void * userptr)	throw()
{
	// copy the parameters
	this->callback	= callback;
	this->userptr	= userptr;

	// start the timeout
	timeout.start(delaygen.current(), this, NULL);
	// return no error
	return netif_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool netif_stat_watch_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// get the new result
	netif_stat_arr_t stat_arr	= netif_stat_helper_t::all_netif_stat();
	
	// notify the caller
	bool	tokeep	= notify_callback(stat_arr);
	if( !tokeep )	return false;

	// set the next timer
	timeout.change_period(delaygen.pre_inc());
	// return a 'tokeep'
	return true;
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
bool 	netif_stat_watch_t::notify_callback(const netif_stat_arr_t &netif_stat_arr)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_netif_stat_watch_cb(userptr, *this, netif_stat_arr);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END



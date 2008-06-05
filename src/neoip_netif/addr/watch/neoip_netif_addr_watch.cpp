/*! \file
    \brief Definition of the netif_addr_watch_t

\par Implementation notes
\ref netif_addr_watch_t periodically does a netif_addr_t::get_all_netif() and 
notify a callback in case of change.

\par Possible Improvement
- Linux allows to be notified immediatly in case of change with the netlink socket
  - this may be coded later to reduce latency

*/

/* system include */
/* local include */
#include "neoip_netif_addr_watch.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;



// definition of \ref netif_addr_watch_t constant
const delaygen_arg_t	netif_addr_watch_t::DELAYGEN_ARG_DFL	= delaygen_regular_arg_t()
									.period(delay_t::from_sec(3*60));	
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
netif_addr_watch_t::netif_addr_watch_t()		throw()
{
	// configure the delaygen to launch the evaluation
	delaygen	= delaygen_t(DELAYGEN_ARG_DFL);	
}

/** \brief Desstructor
 */
netif_addr_watch_t::~netif_addr_watch_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 Start() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief start the actions
 */
netif_err_t netif_addr_watch_t::start(netif_addr_watch_cb_t *callback, void * userptr)	throw()
{
	// copy the parameters
	this->callback	= callback;
	this->userptr	= userptr;
	// get the first result
	current_res	= netif_addr_t::get_all_netif();

	// start the timeout
	timeout.start(delaygen.current(), this, NULL);
	// return no error
	return netif_err_t::OK;
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
bool 	netif_addr_watch_t::notify_callback(const netif_addr_arr_t &netif_addr_arr)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_netif_addr_watch_cb(userptr, *this, netif_addr_arr);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool netif_addr_watch_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	netif_addr_arr_t	new_res;
	// log to debug
	KLOG_DBG("enter");

	// get the new result
	new_res	= netif_addr_t::get_all_netif();
	
	// if the new result is different from the current one, notify the change
	if( new_res != current_res ){
		// notify the caller
		bool	tokeep	= notify_callback(new_res);
		if( !tokeep )	return false;
		// update the current_res
		current_res	= new_res;
	}

	// set the next timer
	timeout.change_period(delaygen.pre_inc());
	// return a 'tokeep'
	return true;
}




NEOIP_NAMESPACE_END



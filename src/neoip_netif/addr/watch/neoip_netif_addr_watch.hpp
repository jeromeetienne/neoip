/*! \file
    \brief Declaration of the netif_addr_watch_t
    
*/


#ifndef __NEOIP_NETIF_ADDR_WATCH_HPP__ 
#define __NEOIP_NETIF_ADDR_WATCH_HPP__ 
/* system include */
/* local include */
#include "neoip_netif_addr_watch_cb.hpp"
#include "neoip_netif_addr.hpp"
#include "neoip_netif_err.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Perform a upd request and notify the result
 */
class netif_addr_watch_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the delaygen to trigger the evaluation
	static const delaygen_arg_t	DELAYGEN_ARG_DFL;	
private:
	netif_addr_arr_t	current_res;	//!< the current netif_addr_arr_t

	/*************** Callback	***************************************/
	netif_addr_watch_cb_t*	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const netif_addr_arr_t &netif_addr_arr)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

	/*************** rxmit timer	***************************************/
	delaygen_t		delaygen;	//!< the delay_t generator for the rxmit
	timeout_t		timeout;	//!< to periodically send packet over the udp_client_t
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	netif_addr_watch_t()		throw();
	~netif_addr_watch_t()		throw();

	/*************** Setup function	***************************************/
	netif_err_t	start(netif_addr_watch_cb_t *callback, void * userptr)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_ADDR_WATCH_HPP__  */




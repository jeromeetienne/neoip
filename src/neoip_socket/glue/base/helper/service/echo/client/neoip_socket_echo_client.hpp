/*! \file
    \brief Header of the test of socket_client_t
*/


#ifndef __NEOIP_SOCKET_ECHO_CLIENT_HPP__ 
#define __NEOIP_SOCKET_ECHO_CLIENT_HPP__ 
/* system include */
/* local include */
#include "neoip_socket_echo_client_cb.hpp"
#include "neoip_socket_client_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	socket_itor_arg_t;
class	pkt_t;

/** \brief Class which implement echo client
 */
class socket_echo_client_t : NEOIP_COPY_CTOR_DENY, private socket_client_cb_t, private timeout_cb_t {
private:	
	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;		//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;		//!< to periodically send the packet
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** Packet building	*******************************/
	pkt_t		build_echo_request()				throw();

	/*************** client	***********************************************/
	socket_client_t*socket_client;
	bool		neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
					, const socket_event_t &socket_event)	throw();

	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();

	/*************** callback stuff	***************************************/
	socket_echo_client_cb_t*callback;	//!< the callback to notify
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const socket_err_t &socket_err)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_echo_client_t()		throw();
	~socket_echo_client_t()		throw();

	/*************** Setup function	***************************************/
	socket_err_t	start(const socket_itor_arg_t &itor_arg, socket_echo_client_cb_t *callback
							, void *userptr)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ECHO_CLIENT_HPP__  */




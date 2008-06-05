/*! \file
    \brief Header of the test of socket_client_t
*/


#ifndef __NEOIP_SOCKET_ECHO_SERVER_CNX_HPP__ 
#define __NEOIP_SOCKET_ECHO_SERVER_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_socket_full_cb.hpp"
#include "neoip_socket_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	socket_echo_server_t;
class	pkt_t;

/** \brief Class which implement a echo server
 */
class socket_echo_server_cnx_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t {
private:
	socket_echo_server_t *	echo_server;	//!< backpointer on the echo_server
	/*************** socket_full_t	***************************************/
	socket_full_t *	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw();
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)	throw();

public:
	/*************** ctor/dtor	***************************************/
	socket_echo_server_cnx_t(socket_echo_server_t *echo_server)		throw();
	~socket_echo_server_cnx_t()						throw();
	
	/*************** Setup function	***************************************/
	socket_err_t	start(socket_full_t *socket_full)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ECHO_SERVER_CNX_HPP__  */




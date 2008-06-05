/*! \file
    \brief Header of the socket_client_t
*/


#ifndef __NEOIP_SOCKET_CLIENT_CB_HPP__ 
#define __NEOIP_SOCKET_CLIENT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class socket_client_t;
class socket_event_t;

/** \brief the callback class for \ref socket_client_t
 */
class socket_client_cb_t {
public:
	virtual bool neoip_socket_client_event_cb(void *cb_userptr, socket_client_t &cb_socket_client
					, const socket_event_t &socket_event)	throw() = 0;
	virtual ~socket_client_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_CLIENT_CB_HPP__  */




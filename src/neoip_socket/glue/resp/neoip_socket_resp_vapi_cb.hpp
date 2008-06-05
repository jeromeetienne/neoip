/*! \file
    \brief Header of the socket_resp_t
*/


#ifndef __NEOIP_SOCKET_RESP_VAPI_CB_HPP__ 
#define __NEOIP_SOCKET_RESP_VAPI_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class socket_resp_vapi_t;
class socket_event_t;

/** \brief the callback class for \ref socket_resp_t
 */
class socket_resp_vapi_cb_t {
public:
	virtual bool neoip_socket_resp_vapi_cb(void *cb_userptr, socket_resp_vapi_t &cb_resp_vapi
					, const socket_event_t &socket_event)	throw() = 0;
	virtual ~socket_resp_vapi_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_RESP_VAPI_CB_HPP__  */




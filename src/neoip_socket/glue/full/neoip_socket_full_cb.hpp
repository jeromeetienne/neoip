/*! \file
    \brief Header of the socket_full_t
*/


#ifndef __NEOIP_SOCKET_FULL_CB_HPP__ 
#define __NEOIP_SOCKET_FULL_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class socket_full_t;
class socket_event_t;

/** \brief the callback class for \ref socket_full_t
 */
class socket_full_cb_t {
public:
	/** \brief to receive the event from socket_full_t
	 */
	virtual bool neoip_socket_full_event_cb(void *cb_userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)
							throw() = 0;
	virtual ~socket_full_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_FULL_CB_HPP__  */




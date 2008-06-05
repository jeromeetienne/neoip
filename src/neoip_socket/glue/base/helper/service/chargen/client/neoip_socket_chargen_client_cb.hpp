/*! \file
    \brief Header of the socket_chargen_client_t
*/


#ifndef __NEOIP_SOCKET_CHARGEN_CLIENT_CB_HPP__ 
#define __NEOIP_SOCKET_CHARGEN_CLIENT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	socket_chargen_client_t;
class	socket_err_t;

/** \brief the callback class for \ref socket_chargen_client_t
 */
class socket_chargen_client_cb_t {
public:
	virtual bool neoip_socket_chargen_client_cb(void *cb_userptr
					, socket_chargen_client_t &cb_chargen_client
					, const socket_err_t &socket_err)	throw() = 0;
	// virtual destructor
	virtual ~socket_chargen_client_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_CHARGEN_CLIENT_CB_HPP__  */




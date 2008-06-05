/*! \file
    \brief Header of the tcp_chargen_client_t
*/


#ifndef __NEOIP_TCP_CHARGEN_CLIENT_CB_HPP__ 
#define __NEOIP_TCP_CHARGEN_CLIENT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	tcp_chargen_client_t;
class	inet_err_t;

/** \brief the callback class for \ref tcp_chargen_client_t
 */
class tcp_chargen_client_cb_t {
public:
	virtual bool neoip_tcp_chargen_client_cb(void *cb_userptr
					, tcp_chargen_client_t &cb_chargen_client
					, const inet_err_t &inet_err)	throw() = 0;
	// virtual destructor
	virtual ~tcp_chargen_client_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_CHARGEN_CLIENT_CB_HPP__  */




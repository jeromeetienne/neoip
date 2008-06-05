/*! \file
    \brief Declaration of the upnp_call_extipaddr_t callback
    
*/


#ifndef __NEOIP_UPNP_CALL_EXTIPADDR_CB_HPP__ 
#define __NEOIP_UPNP_CALL_EXTIPADDR_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	upnp_call_extipaddr_t;
class	upnp_err_t;
class	ip_addr_t;

/** \brief the callback class for upnp_call_extipaddr_t
 */
class upnp_call_extipaddr_cb_t {
public:
	/** \brief callback notified by \ref upnp_call_extipaddr_t when completed
	 */
	virtual bool neoip_upnp_call_extipaddr_cb(void *cb_userptr, upnp_call_extipaddr_t &cb_call_extipaddr
				, const upnp_err_t &upnp_err, const ip_addr_t &ipaddr)	throw() = 0;
	virtual ~upnp_call_extipaddr_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_CALL_EXTIPADDR_CB_HPP__  */




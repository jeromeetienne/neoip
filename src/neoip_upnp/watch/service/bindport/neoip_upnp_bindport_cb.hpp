/*! \file
    \brief Declaration of the upnp_bindport_t callback
    
*/


#ifndef __NEOIP_UPNP_BINDPORT_CB_HPP__ 
#define __NEOIP_UPNP_BINDPORT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	upnp_bindport_t;
class	upnp_err_t;
class	ipport_addr_t;

/** \brief the callback class for upnp_bindport_t
 */
class upnp_bindport_cb_t {
public:
	/** \brief callback notified by \ref upnp_bindport_t on completion
	 */
	virtual bool neoip_upnp_bindport_cb(void *cb_userptr, upnp_bindport_t &cb_upnp_bindport
					, const upnp_err_t &upnp_err
					, const ipport_addr_t &ipport_pview)	throw() = 0;
	virtual ~upnp_bindport_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_BINDPORT_CB_HPP__  */




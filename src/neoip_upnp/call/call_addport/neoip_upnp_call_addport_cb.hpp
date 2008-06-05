/*! \file
    \brief Declaration of the upnp_call_addport_t callback
    
*/


#ifndef __NEOIP_UPNP_CALL_ADDPORT_CB_HPP__ 
#define __NEOIP_UPNP_CALL_ADDPORT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	upnp_call_addport_t;
class	upnp_err_t;

/** \brief the callback class for upnp_call_addport_t
 */
class upnp_call_addport_cb_t {
public:
	/** \brief callback notified by \ref upnp_call_addport_t when completed
	 */
	virtual bool neoip_upnp_call_addport_cb(void *cb_userptr, upnp_call_addport_t &cb_call_addport
						, const upnp_err_t &upnp_err)	throw() = 0;
	virtual ~upnp_call_addport_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_CALL_ADDPORT_CB_HPP__  */




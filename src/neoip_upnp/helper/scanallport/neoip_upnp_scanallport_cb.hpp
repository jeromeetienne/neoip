/*! \file
    \brief Declaration of the upnp_scanallport_t callback
    
*/


#ifndef __NEOIP_UPNP_SCANALLPORT_CB_HPP__ 
#define __NEOIP_UPNP_SCANALLPORT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	upnp_scanallport_t;
class	upnp_err_t;

/** \brief the callback class for upnp_scanallport_t
 */
class upnp_scanallport_cb_t {
public:
	/** \brief callback notified by \ref upnp_scanallport_t on completion
	 */
	virtual bool neoip_upnp_scanallport_cb(void *cb_userptr, upnp_scanallport_t &cb_upnp_scanallport
				, const upnp_err_t &upnp_err)	throw() = 0;
	virtual ~upnp_scanallport_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_SCANALLPORT_CB_HPP__  */




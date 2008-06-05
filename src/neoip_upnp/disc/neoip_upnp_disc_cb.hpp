/*! \file
    \brief Declaration of the upnp_disc_t
    
*/


#ifndef __NEOIP_UPNP_DISC_CB_HPP__ 
#define __NEOIP_UPNP_DISC_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// List of forward declaration
class	upnp_disc_t;
class	upnp_disc_res_t;
class	upnp_err_t;

/** \brief the discback class for http_client_t
 */
class upnp_disc_cb_t {
public:
	/** \brief discback notified by \ref upnp_disc_t on completion
	 * 
	 * @upnp_err	the error if one occured, else upnp_err_t::OK
	 * @disc_res	the upnp_disc_res_t storing the information discovered
	 */
	virtual bool neoip_upnp_disc_cb(void *cb_userptr, upnp_disc_t &cb_disc, const upnp_err_t &upnp_err
					, const upnp_disc_res_t &disc_res)		throw() = 0;
	// virtual destructor
	virtual ~upnp_disc_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_DISC_CB_HPP__  */




/*! \file
    \brief Declaration of the upnp_call_t
    
*/


#ifndef __NEOIP_UPNP_CALL_CB_HPP__ 
#define __NEOIP_UPNP_CALL_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// List of forward declaration
class	upnp_call_t;
class	upnp_err_t;
class	strvar_db_t;

/** \brief the callback class for http_client_t
 */
class upnp_call_cb_t {
public:
	/** \brief callback notified by \ref upnp_call_t on completion
	 * 
	 * @upnp_err	the error if one occured, else upnp_err_t::OK
	 * @strvar_db	the variable found in the response
	 */
	virtual bool neoip_upnp_call_cb(void *cb_userptr, upnp_call_t &cb_call, const upnp_err_t &upnp_err
					, const strvar_db_t &strvar_db)		throw() = 0;
	// virtual destructor
	virtual ~upnp_call_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_CALL_CB_HPP__  */




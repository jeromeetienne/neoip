/*! \file
    \brief Declaration of the upnp_call_addport_t
    
*/


#ifndef __NEOIP_UPNP_CALL_ADDPORT_HPP__ 
#define __NEOIP_UPNP_CALL_ADDPORT_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_call_addport_cb.hpp"
#include "neoip_upnp_call_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	upnp_disc_res_t;
class	upnp_call_profile_t;
class	upnp_sockfam_t;
class	delay_t;

/** \brief handle the listener of the nslan
 */
class upnp_call_addport_t : NEOIP_COPY_CTOR_DENY, private upnp_call_cb_t {
private:
	/*************** upnp_call_t	***************************************/
	upnp_call_t *	upnp_call;
	bool 		neoip_upnp_call_cb(void *cb_userptr, upnp_call_t &cb_call, const upnp_err_t &upnp_err
					, const strvar_db_t &replied_var)	throw();

	/*************** callback stuff	***************************************/
	upnp_call_addport_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const upnp_err_t &upnp_err)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	upnp_call_addport_t()	throw();
	~upnp_call_addport_t()	throw();

	/*************** setup function	***************************************/
	upnp_call_addport_t &set_profile(const upnp_call_profile_t &profile)	throw();	
	upnp_err_t	start(const upnp_disc_res_t &disc_res, uint16_t port_pview, uint16_t port_lview
				, const upnp_sockfam_t &sock_fam, const delay_t &lease_delay
				, const std::string &description_str
				, upnp_call_addport_cb_t *callback, void * userptr)	throw();
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_CALL_ADDPORT_HPP__ 




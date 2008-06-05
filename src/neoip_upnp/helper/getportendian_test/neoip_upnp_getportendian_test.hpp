/*! \file
    \brief Declaration of the upnp_getportendian_test_t
    
*/


#ifndef __NEOIP_UPNP_GETPORTENDIAN_TEST_HPP__ 
#define __NEOIP_UPNP_GETPORTENDIAN_TEST_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_getportendian_test_profile.hpp"
#include "neoip_upnp_getportendian_test_cb.hpp"
#include "neoip_upnp_disc_res.hpp"
#include "neoip_upnp_call_getport_cb.hpp"
#include "neoip_upnp_call_addport_cb.hpp"
#include "neoip_upnp_sockfam.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief handle the listener of the nslan
 */
class upnp_getportendian_test_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
					, private upnp_call_getport_cb_t
					, private upnp_call_addport_cb_t {
private:
	upnp_getportendian_test_profile_t	profile;//!< the profile to use for this object
	upnp_disc_res_t		upnp_disc_res;	//!< the upnp_disc_res_t
	uint16_t		port_pview;	//!< the current port_pview
	uint16_t		port_lview;	//!< the current port_lview
	size_t			getport_nbretry;//!< the number of getport which have been started
	bool			is_bound;	//!< true if the upnp_call_addport_t succeed, false otherwise
	bool			m_is_revendian;	//!< true if the upnp router is revendian, false otherwise
	
	/*************** upnp_getport_t main callback	***********************/
	bool	neoip_upnp_call_getport_cb(void *cb_userptr, upnp_call_getport_t &cb_call_getport
						, const upnp_err_t &upnp_err)	throw();

	/*************** upnp_getport_t for itor phase	***********************/
	upnp_err_t		itor_getport_launch()	throw();
	upnp_call_getport_t *	itor_getport_norendian;
	upnp_call_getport_t *	itor_getport_revendian;
	bool			itor_getport_cb(void *cb_userptr, upnp_call_getport_t &cb_call_getport
						, const upnp_err_t &upnp_err)	throw();
							
	/*************** upnp_addport_t	***************************************/
	upnp_err_t		call_addport_launch()	throw();
	upnp_call_addport_t *	call_addport;
	bool 			neoip_upnp_call_addport_cb(void *cb_userptr, upnp_call_addport_t &cb_call_addport
						, const upnp_err_t &upnp_err)	throw();

	/*************** upnp_getport_t for full phase	***********************/
	upnp_err_t		full_getport_launch()	throw();
	upnp_call_getport_t *	full_getport_norendian;
	upnp_call_getport_t *	full_getport_revendian;
	bool			full_getport_cb(void *cb_userptr, upnp_call_getport_t &cb_call_getport
						, const upnp_err_t &upnp_err)	throw();

	/*************** itor_expire_timeout	*******************************/
	timeout_t	expire_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** callback stuff	***************************************/
	upnp_getportendian_test_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const upnp_err_t &upnp_err)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	upnp_getportendian_test_t()	throw();
	~upnp_getportendian_test_t()	throw();

	/*************** setup function	***************************************/
	upnp_getportendian_test_t&set_profile(const upnp_getportendian_test_profile_t &profile)	throw();
	upnp_err_t	start(const upnp_disc_res_t &disc_res, upnp_getportendian_test_cb_t *callback
							, void * userptr)	throw();
							
	/*************** query function	***************************************/
	bool		is_revendian()	const throw()	{ return m_is_revendian;	}
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_GETPORTENDIAN_TEST_HPP__ 




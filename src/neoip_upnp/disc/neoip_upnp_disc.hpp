/*! \file
    \brief Declaration of the upnp_disc_t
    
*/


#ifndef __NEOIP_UPNP_DISC_HPP__ 
#define __NEOIP_UPNP_DISC_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_disc_wikidbg.hpp"
#include "neoip_upnp_disc_profile.hpp"
#include "neoip_upnp_disc_cb.hpp"
#include "neoip_upnp_disc_res.hpp"
#include "neoip_upnp_getportendian_test_cb.hpp"
#include "neoip_nudp_cb.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_xml_except.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	upnp_err_t;
class	http_uri_t;
class	http_reqhd_t;
class	pkt_t;
class	datum_t;

/** \brief handle the discovery for upnp
 */
class upnp_disc_t : NEOIP_COPY_CTOR_DENY, private http_sclient_cb_t, private nudp_cb_t
					, private timeout_cb_t, private upnp_getportendian_test_cb_t
					, private wikidbg_obj_t<upnp_disc_t, upnp_disc_wikidbg_init> {
private:
	upnp_disc_profile_t	profile;	//!< the current profile for this object
	upnp_disc_res_t		current_res;	//!< the current upnp_disc_res_t

	/*************** Internal function	*******************************/
	pkt_t		build_upnp_disc_req()					const throw();
	upnp_err_t	parse_upnp_disc_rep(pkt_t &pkt)				throw();
	upnp_err_t	parse_upnp_http_rep(const http_sclient_res_t &sclient_res)	throw(xml_except_t);

	/*************** main timeout_t callback	***********************/
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
					
	/*************** nudprxmit_timeout	*******************************/
	delaygen_t	nudprxmit_delaygen;
	timeout_t	nudprxmit_timeout;
	bool 		nudprxmit_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** nudp_t stuff	***************************************/
	nudp_t *	nudp;
	bool		neoip_inet_nudp_event_cb(void *cb_userptr, nudp_t &cb_nudp, pkt_t &pkt
						, const ipport_addr_t &local_addr
						, const ipport_addr_t &remote_addr)		throw();
						
	/*************** http_sclient	***************************************/
	http_sclient_t *http_sclient;
	bool 		neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
						, const http_sclient_res_t &sclient_res)	throw();

	/*************** upnp_getportendian_test_t	*******************************/
	upnp_getportendian_test_t *	getportendian_test;					
	bool 		neoip_upnp_getportendian_test_cb(void *cb_userptr, upnp_getportendian_test_t &cb_getportendian_test
							, const upnp_err_t &upnp_err)	throw();
	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;
	bool 		expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** callback stuff	***************************************/
	upnp_disc_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const upnp_err_t &upnp_err, const upnp_disc_res_t &disc_res)throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	upnp_disc_t()	throw();
	~upnp_disc_t()	throw();

	/*************** setup function	***************************************/
	upnp_disc_t &	set_profile(const upnp_disc_profile_t &profile)		throw();
	upnp_err_t	start(const std::string &m_service_name, upnp_disc_cb_t *callback
							, void * userptr)	throw();

	/*************** query function	***************************************/
	const std::string &	service_name()	const throw()	{ return current_res.service_name();	}

	/*************** List of friend class	*******************************/
	friend class	upnp_disc_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_DISC_HPP__ 




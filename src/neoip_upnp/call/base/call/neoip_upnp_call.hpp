/*! \file
    \brief Declaration of the upnp_call_t
    
*/


#ifndef __NEOIP_UPNP_CALL_HPP__ 
#define __NEOIP_UPNP_CALL_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_call_cb.hpp"
#include "neoip_upnp_call_profile.hpp"
#include "neoip_upnp_call_wikidbg.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	upnp_err_t;
class	http_uri_t;
class	strvar_db_t;
class	http_reqhd_t;
class	pkt_t;
class	datum_t;
class	xml_parse_t;

/** \brief handle the listener of the nslan
 */
class upnp_call_t : NEOIP_COPY_CTOR_DENY, private http_sclient_cb_t, private timeout_cb_t
			, private wikidbg_obj_t<upnp_call_t, upnp_call_wikidbg_init>  {
private:
	upnp_call_profile_t	profile;	//!< the current profile for this object
	http_uri_t		m_control_uri;	//!< the control_uri for this call
	std::string		m_service_name;	//!< the service_name called
	std::string		m_method_name;	//!< the method_name called
	strvar_db_t		m_strvar_db;	//!< the strvar_db used for the call
		
	/*************** build function	***************************************/
	pkt_t		build_soap_req(const std::string &service_name, const std::string &method_name
						, const strvar_db_t &variable)	const throw();
	http_reqhd_t	build_soap_reqhd(const http_uri_t &control_uri, const std::string &service_name
						, const std::string &method_name
						, const datum_t &data2post)	const throw();
	/*************** parse function	***************************************/
	upnp_err_t	parse_soap_rep(const datum_t &xml_datum, strvar_db_t &strvar_db)	const throw();
	upnp_err_t	parse_soap_rep_success(const datum_t &xml_datum, xml_parse_t &xml_parse
								, strvar_db_t &strvar_db)	const throw();
	upnp_err_t	parse_soap_rep_fault(const datum_t &xml_datum, xml_parse_t &xml_parse
								, strvar_db_t &strvar_db)	const throw();
						
	/*************** http_sclient	***************************************/
	http_sclient_t *http_sclient;
	bool 		neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &sclient_res)	throw();
	upnp_err_t	launch_http_sclient()					throw();

	/*************** main timeout_t callback	***********************/
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;
	bool 		expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** expire_timeout	***************************************/
	delaygen_t	httperr_retry_delaygen;	//!< the delay_t generator for retrying 	
	timeout_t	httperr_retry_timeout;
	bool 		httperr_retry_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	size_t		httperr_retry_nbcur;
	bool		handle_http_sclient_error()				throw();
	
	/*************** callback stuff	***************************************/
	upnp_call_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const upnp_err_t &upnp_err, const strvar_db_t &strvar_db)throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	upnp_call_t()	throw();
	~upnp_call_t()	throw();

	/*************** setup function	***************************************/
	upnp_call_t &	set_profile(const upnp_call_profile_t &profile)		throw();	
	upnp_err_t	start(const http_uri_t &control_uri, const std::string &m_service_name
				, const std::string &m_method_name, const strvar_db_t &m_strvar_db
				, upnp_call_cb_t *callback, void * userptr)	throw();

	/*************** Query function	***************************************/
	const upnp_call_profile_t &get_profile()const throw()	{ return profile;	}
	const http_uri_t &	control_uri()	const throw()	{ return m_control_uri;	}
	const std::string &	service_name()	const throw()	{ return m_service_name;}
	const std::string &	method_name()	const throw()	{ return m_method_name;	}
	const strvar_db_t &	strvar_db()	const throw()	{ return m_strvar_db;	}

	/*************** List of friend class	*******************************/
	friend class	upnp_call_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_CALL_HPP__ 




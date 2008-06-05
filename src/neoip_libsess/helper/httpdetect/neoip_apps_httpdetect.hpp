/*! \file
    \brief Header of the \ref apps_httpdetect_t
    
*/


#ifndef __NEOIP_APPS_HTTPDETECT_HPP__ 
#define __NEOIP_APPS_HTTPDETECT_HPP__ 
/* system include */
/* local include */
#include "neoip_apps_httpdetect_wikidbg.hpp"
#include "neoip_xmlrpc_resp_cb.hpp"
#include "neoip_http_sresp_cb.hpp"		// TODO to remove
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	libsess_err_t;
class	http_listener_t;
class	xmlrpc_listener_t;

/** \brief Handle the apps_httpdetect_t
 */
class apps_httpdetect_t : NEOIP_COPY_CTOR_DENY, private xmlrpc_resp_cb_t
		, private http_sresp_cb_t	// TODO to remove
		, private wikidbg_obj_t<apps_httpdetect_t, apps_httpdetect_wikidbg_init> {
private:
	xmlrpc_listener_t *	m_xmlrpc_listener;
	std::string		m_canon_name;
	std::string		m_version;

	/*************** xmlrpc_resp_t	***************************************/
	xmlrpc_resp_t *	m_xmlrpc_resp;
	bool neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw();

	/*************** Handle the xmlrpc method call	***********************/
	xmlrpc_err_t	handle_call_probe_apps(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw();

	/*************** webdetect stuff	*******************************/
	// TODO to remove
	http_sresp_t *	m_http_sresp;
	bool		neoip_http_sresp_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
						, http_sresp_ctx_t &sresp_ctx)	throw();
public:
	/*************** ctor/dtor	***************************************/
	apps_httpdetect_t()	throw();
	~apps_httpdetect_t()	throw();
	
	/*************** Setup function	***************************************/
	libsess_err_t	start(http_listener_t *http_listener, const std::string &p_canon_name
					, const std::string &p_version)		throw();
	
	/*************** List of friend class	*******************************/
	friend class	apps_httpdetect_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_APPS_HTTPDETECT_HPP__  */











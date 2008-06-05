/*! \file
    \brief Header of the \ref casti_ctrl_wpage_t
    
*/


#ifndef __NEOIP_CASTI_CTRL_WPAGE_HPP__ 
#define __NEOIP_CASTI_CTRL_WPAGE_HPP__ 
/* system include */
/* local include */
#include "neoip_casti_ctrl_wpage_wikidbg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_xmlrpc_resp_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casti_apps_t;
class	xmlrpc_listener_t;

/** \brief Handle the control via webpage
 */
class casti_ctrl_wpage_t : NEOIP_COPY_CTOR_DENY, private xmlrpc_resp_cb_t
		, private wikidbg_obj_t<casti_ctrl_wpage_t, casti_ctrl_wpage_wikidbg_init> {
private:
	casti_apps_t *		m_casti_apps;
	xmlrpc_listener_t *	m_xmlrpc_listener;

	/*************** xmlrpc_resp_t	***************************************/
	xmlrpc_resp_t *	m_xmlrpc_resp;
	bool neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw();

	/*************** Handle the xmlrpc method call	***********************/
	xmlrpc_err_t	xmlrpc_call_request_stream(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw();
	xmlrpc_err_t	xmlrpc_call_release_stream(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw();
public:
	/*************** ctor/dtor	***************************************/
	casti_ctrl_wpage_t()		throw();
	~casti_ctrl_wpage_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(casti_apps_t *m_casti_apps)		throw();

	/*************** List of friend class	*******************************/
	friend class	casti_ctrl_wpage_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_CTRL_WPAGE_HPP__  */











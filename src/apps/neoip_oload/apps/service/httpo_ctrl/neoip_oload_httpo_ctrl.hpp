/*! \file
    \brief Header of the \ref oload_httpo_ctrl_t
    
*/


#ifndef __NEOIP_OLOAD_HTTPO_CTRL_HPP__ 
#define __NEOIP_OLOAD_HTTPO_CTRL_HPP__ 
/* system include */
/* local include */
#include "neoip_oload_httpo_ctrl_wikidbg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_xmlrpc_resp_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	oload_apps_t;
class	bt_httpo_full_t;
class	xmlrpc_listener_t;
class	http_uri_t;

/** \brief Handle the control via webpage
 */
class oload_httpo_ctrl_t : NEOIP_COPY_CTOR_DENY, private xmlrpc_resp_cb_t
		, private wikidbg_obj_t<oload_httpo_ctrl_t, oload_httpo_ctrl_wikidbg_init> {
private:
	oload_apps_t *		m_oload_apps;
	xmlrpc_listener_t *	m_xmlrpc_listener;

	/*************** internal function	*******************************/
	bt_httpo_full_t *	httpo_full_by_id(const std::string &httpo_full_id)	throw();

	/*************** xmlrpc_resp_t	***************************************/
	xmlrpc_resp_t *	m_xmlrpc_resp;
	bool 	neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw();

	/*************** Handle the xmlrpc method call	***********************/
	xmlrpc_err_t	xmlrpc_call_set_maxrate(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw();
public:
	/*************** ctor/dtor	***************************************/
	oload_httpo_ctrl_t()		throw();
	~oload_httpo_ctrl_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(oload_apps_t *m_oload_apps)		throw();

	/*************** List of friend class	*******************************/
	friend class	oload_httpo_ctrl_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_HTTPO_CTRL_HPP__  */











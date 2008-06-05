/*! \file
    \brief Header of the \ref bt_cast_mdata_server_t
    
*/


#ifndef __NEOIP_BT_CAST_MDATA_SERVER_HPP__ 
#define __NEOIP_BT_CAST_MDATA_SERVER_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_cast_mdata_server_wikidbg.hpp"
#include "neoip_bt_cast_mdata_server_cb.hpp"
#include "neoip_xmlrpc_resp_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casti_apps_t;
class	http_listener_t;
class	xmlrpc_listener_t;

/** \brief Handle the control via webpage
 */
class bt_cast_mdata_server_t : NEOIP_COPY_CTOR_DENY, private xmlrpc_resp_cb_t
			, private wikidbg_obj_t<bt_cast_mdata_server_t, bt_cast_mdata_server_wikidbg_init> {
private:
	xmlrpc_listener_t *	m_xmlrpc_listener;

	/*************** xmlrpc_resp_t	***************************************/
	xmlrpc_resp_t *	m_xmlrpc_resp;
	bool neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw();

	/*************** Handle the xmlrpc method call	***********************/
	xmlrpc_err_t	xmlrpc_call_get_cast_mdata(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw();

	/*************** callback stuff	***************************************/
	bt_cast_mdata_server_cb_t *callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_cast_id_t &cast_id
					, bt_cast_mdata_t *cast_mdata_out)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_mdata_server_t()		throw();
	~bt_cast_mdata_server_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(http_listener_t *m_http_listener
				, bt_cast_mdata_server_cb_t *callback, void *userptr)	throw();

	/*************** Query function	***************************************/
	xmlrpc_listener_t * xmlrpc_listener()	const throw() { return m_xmlrpc_listener;	}
	
	/*************** List of friend class	*******************************/
	friend class	bt_cast_mdata_server_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_MDATA_SERVER_HPP__  */











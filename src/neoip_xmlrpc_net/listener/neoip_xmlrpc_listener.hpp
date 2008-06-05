/*! \file
    \brief Header of the \ref xmlrpc_listener_t
    
*/


#ifndef __NEOIP_XMLRPC_LISTENER_HPP__ 
#define __NEOIP_XMLRPC_LISTENER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_xmlrpc_listener_wikidbg.hpp"
#include "neoip_http_sresp_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	xmlrpc_resp_t;
class	xmlrpc_parse_t;
class	xmlrpc_err_t;
class	http_listener_t;
class	http_uri_t;

/** \brief Handle the webdetect part for the bt_oload_stat_t
 */
class xmlrpc_listener_t : NEOIP_COPY_CTOR_DENY, private http_sresp_cb_t
		, private wikidbg_obj_t<xmlrpc_listener_t, xmlrpc_listener_wikidbg_init> {
private:
	http_listener_t *	m_http_listener;

	/*************** http_sresp_t callback	*******************************/
	bool		neoip_http_sresp_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
						, http_sresp_ctx_t &sresp_ctx)	throw();
	/*************** http_sresp_t for jsrest	***********************/
	http_sresp_t *	m_sresp_jsrest;
	bool		sresp_jsrest_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
						, http_sresp_ctx_t &sresp_ctx)	throw();
						
	/*************** http_sresp_t for xmlrpc	***********************/
	http_sresp_t *	m_sresp_xmlrpc;
	bool		sresp_xmlrpc_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
						, http_sresp_ctx_t &sresp_ctx)	throw();
	std::string	parse_xmlrpc_call(const datum_t &xmlenc_datum)		throw();

	/*************** store the xmlrpc_resp_t	***********************/
	std::list<xmlrpc_resp_t *>	resp_db;
	void resp_dolink(xmlrpc_resp_t *resp) 	throw()	{ resp_db.push_back(resp);	}
	void resp_unlink(xmlrpc_resp_t *resp)	throw()	{ resp_db.remove(resp);		}
	xmlrpc_resp_t *	resp_by_method_name(const std::string &method_name)	const throw();
public:
	/*************** ctor/dtor	***************************************/
	xmlrpc_listener_t()		throw();
	~xmlrpc_listener_t()		throw();

	/*************** Setup function	***************************************/
	xmlrpc_err_t	start(http_listener_t *http_listener, const http_uri_t &xmlrpc_uri
					, const http_uri_t &jsrest_uri)		throw();
	
	/*************** Query function	***************************************/
	http_listener_t *	http_listener()	const throw()	{ return m_http_listener;	}
	const http_uri_t &	xmlrpc_uri()	const throw();
	const http_uri_t &	jsrest_uri()	const throw();
	

	/*************** List of friend class	*******************************/
	friend class	xmlrpc_listener_wikidbg_t;
	friend class	xmlrpc_resp_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_LISTENER_HPP__ */











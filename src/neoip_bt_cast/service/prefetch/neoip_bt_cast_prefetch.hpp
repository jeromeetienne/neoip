/*! \file
    \brief Header of the \ref bt_cast_prefetch_t
    
*/


#ifndef __NEOIP_BT_CAST_PREFETCH_HPP__ 
#define __NEOIP_BT_CAST_PREFETCH_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_cast_prefetch_wikidbg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_xmlrpc_resp_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_cast_prefetch_cnx_t;
class	xmlrpc_listener_t;
class	http_listener_t;
class	http_uri_t;

/** \brief Handle the webdetect part for the bt_oload_stat_t
 */
class bt_cast_prefetch_t : NEOIP_COPY_CTOR_DENY, private xmlrpc_resp_cb_t
			, private wikidbg_obj_t<bt_cast_prefetch_t, bt_cast_prefetch_wikidbg_init> {
private:
	xmlrpc_listener_t *	m_xmlrpc_listener;
	
	/*************** xmlrpc_resp_t	***************************************/
	xmlrpc_resp_t *	m_xmlrpc_resp;
	bool neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw();

	/*************** Handle the xmlrpc method call	***********************/
	xmlrpc_err_t	xmlrpc_call_create_prefetch(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw();
	xmlrpc_err_t	xmlrpc_call_delete_prefetch(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw();

	/*************** store the bt_cast_prefetch_cnx_t	***********************/
	std::list<bt_cast_prefetch_cnx_t *>	cnx_db;
	void cnx_dolink(bt_cast_prefetch_cnx_t *cnx) 	throw()	{ cnx_db.push_back(cnx);}
	void cnx_unlink(bt_cast_prefetch_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);	}
	bt_cast_prefetch_cnx_t *	cnx_by_http_uri(const http_uri_t &http_uri)	const throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_prefetch_t()		throw();
	~bt_cast_prefetch_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(http_listener_t *http_listener)		throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_cast_prefetch_wikidbg_t;
	friend class	bt_cast_prefetch_cnx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_PREFETCH_HPP__ */











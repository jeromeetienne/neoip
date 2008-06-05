/*! \file
    \brief Header of the \ref bt_cast_mdata_unpublish_t
    
*/


#ifndef __NEOIP_BT_CAST_MDATA_UNPUBLISH_HPP__ 
#define __NEOIP_BT_CAST_MDATA_UNPUBLISH_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_cast_mdata_unpublish_wikidbg.hpp"
#include "neoip_bt_cast_mdata_unpublish_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_xmlrpc_client_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_uri_t;

/** \brief unpublish the bt_cast_mdata_t for the casti_swarm_t
 */
class bt_cast_mdata_unpublish_t : NEOIP_COPY_CTOR_DENY, private xmlrpc_client_cb_t
			, private wikidbg_obj_t<bt_cast_mdata_unpublish_t, bt_cast_mdata_unpublish_wikidbg_init> {
private:
	/*************** xmlrpc_client	***************************************/
	xmlrpc_client_t *m_xmlrpc_client;
	bool		neoip_xmlrpc_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw();
	bt_err_t	handle_xmlrpc_resp(const xmlrpc_err_t &xmlrpc_err
						, const datum_t &xmlrpc_resp)	throw();

	/*************** callback stuff	***************************************/
	bt_cast_mdata_unpublish_cb_t *callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_err_t &bt_err)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_mdata_unpublish_t()	throw();
	~bt_cast_mdata_unpublish_t()	throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(const http_uri_t &mdata_srv_uri, const std::string &cast_name
				, const std::string &cast_privtext
				, bt_cast_mdata_unpublish_cb_t *callback, void *userptr)	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_cast_mdata_unpublish_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_MDATA_UNPUBLISH_HPP__ */











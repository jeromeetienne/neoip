/*! \file
    \brief Declaration of the bt_cast_mdata_client_t
    
*/


#ifndef __NEOIP_BT_CAST_MDATA_CLIENT_HPP__ 
#define __NEOIP_BT_CAST_MDATA_CLIENT_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_cast_mdata_client_cb.hpp"
#include "neoip_xmlrpc_client_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_cast_id_t;
class	bt_err_t;
class	http_reqhd_t;

/** \brief handle the listener of the nslan
 */
class bt_cast_mdata_client_t : NEOIP_COPY_CTOR_DENY, private xmlrpc_client_cb_t {
private:						
	http_uri_t	m_server_uri;

	/*************** Internal function	*******************************/
	bt_err_t	start_common(const http_uri_t &m_server_uri, const datum_t &xmlrpc_call
				, bt_cast_mdata_client_cb_t *callback, void * userptr)	throw();

	/*************** xmlrpc_client	***************************************/
	xmlrpc_client_t *xmlrpc_client;
	bool		neoip_xmlrpc_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw();

	/*************** callback stuff	***************************************/
	bt_cast_mdata_client_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback_success(const bt_cast_mdata_t &cast_mdata)	throw();
	bool		notify_callback_failure(const bt_err_t &bt_err)			throw();
	bool		notify_callback(const bt_err_t &bt_err, const bt_cast_mdata_t &cast_mdata)throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_mdata_client_t()	throw();
	~bt_cast_mdata_client_t()	throw();

	/*************** setup function	***************************************/
	bt_err_t	start(const http_uri_t &m_server_uri, const std::string &m_cast_name
				, const std::string &m_cast_privhash
				, bt_cast_mdata_client_cb_t *callback, void * userptr)	throw();
	bt_err_t	start(const http_uri_t &m_server_uri, const bt_cast_id_t &m_cast_id
				, bt_cast_mdata_client_cb_t *callback, void * userptr)	throw();

	/*************** Query function	***************************************/
	const http_uri_t &	server_uri()	const throw()	{ return m_server_uri;	}
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_BT_CAST_MDATA_CLIENT_HPP__ 




/*! \file
    \brief Declaration of the xmlrpc_client_t
    
*/


#ifndef __NEOIP_XMLRPC_CLIENT_HPP__ 
#define __NEOIP_XMLRPC_CLIENT_HPP__ 
/* system include */
/* local include */
#include "neoip_xmlrpc_client_cb.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	xmlrpc_err_t;
class	http_reqhd_t;

/** \brief handle the listener of the nslan
 */
class xmlrpc_client_t : NEOIP_COPY_CTOR_DENY, private http_sclient_cb_t {
private:						
	http_uri_t	m_server_uri;
	
	/*************** Internal function	*******************************/
	http_reqhd_t	build_http_reqhd(const datum_t &data2post)	const throw();
	
	/*************** http_sclient	***************************************/
	http_sclient_t *http_sclient;
	bool 		neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
						, const http_sclient_res_t &sclient_res)	throw();

	/*************** callback stuff	***************************************/
	xmlrpc_client_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback_success(const datum_t &xmlrpc_resp)	throw();
	bool		notify_callback_failure(const xmlrpc_err_t &xmlrpc_err)	throw();
	bool		notify_callback(const xmlrpc_err_t &xmlrpc_err
						, const datum_t &xmlrpc_resp)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	xmlrpc_client_t()	throw();
	~xmlrpc_client_t()	throw();

	/*************** setup function	***************************************/
	xmlrpc_err_t	start(const http_uri_t &m_server_uri, const datum_t &xmlrpc_call
				, xmlrpc_client_cb_t *callback, void * userptr)	throw();

	/*************** Query function	***************************************/
	const http_uri_t &	server_uri()	const throw()	{ return m_server_uri;	}
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_XMLRPC_CLIENT_HPP__ 




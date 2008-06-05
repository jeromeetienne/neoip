/*! \file
    \brief Declaration of the xmlrpc_client_t

\par Brief Description
\ref xmlrpc_client_t does a single xmlrpc call to a xmlrpc server.

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_client.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_http_sclient_res.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
xmlrpc_client_t::xmlrpc_client_t()		throw()
{
	// zero some fields
	http_sclient	= NULL;
}

/** \brief Desstructor
 */
xmlrpc_client_t::~xmlrpc_client_t()		throw()
{
	// destroy the http_sclient_t if needed
	nipmem_zdelete	http_sclient;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief start function - the generic one used by all the helper one
 */
xmlrpc_err_t xmlrpc_client_t::start(const http_uri_t &m_server_uri, const datum_t &xmlrpc_call
				, xmlrpc_client_cb_t *callback, void * userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->m_server_uri	= m_server_uri;
	this->callback		= callback;
	this->userptr		= userptr;
	

	// build the http_reqhd_t to use for the http_sclient_t
	http_reqhd_t	http_reqhd;
	http_reqhd.method("POST").uri(server_uri());
	http_reqhd.header_db().append("Content-Length", OSTREAMSTR(xmlrpc_call.length()));
	http_reqhd.header_db().append("Content-Type", "text/xml; charset=\"utf-8\"");
	// start the http_sclient_t
	http_err_t	http_err;
	http_sclient	= nipmem_new http_sclient_t();
	http_err	= http_sclient->start(http_reqhd, this, NULL, xmlrpc_call);
	if( http_err.failed() )	return xmlrpc_err_from_http(http_err);

	// return no error
	return xmlrpc_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	xmlrpc_client_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &cb_sclient_res)	throw() 
{
	http_sclient_res_t	sclient_res	= cb_sclient_res;
	// log to debug
	KLOG_DBG("enter http_sclient_res=" << sclient_res);
	KLOG_DBG("reply_body=" << sclient_res.reply_body().to_stdstring());

	// if http_sclient_t failed, notify the caller
	if( !sclient_res.is_post_ok() ){
		std::string	reason	= "http_sclient_res_t not is_post_ok";
		return notify_callback_failure(xmlrpc_err_t(xmlrpc_err_t::ERROR, reason));
	}

	// notify the xmlrpc_resp
	datum_t	xmlrpc_resp	= sclient_res.reply_body().to_datum();
	return 	notify_callback_success(xmlrpc_resp);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Notify the callback when it is success
 */
bool	xmlrpc_client_t::notify_callback_success(const datum_t &xmlrpc_resp)	throw()
{
	return notify_callback(xmlrpc_err_t::OK, xmlrpc_resp);
}

/** \brief Notify the callback when it is failure
 */
bool	xmlrpc_client_t::notify_callback_failure(const xmlrpc_err_t &xmlrpc_err)throw()
{
	return notify_callback(xmlrpc_err, datum_t());
}

/** \brief Notify the callback when it is success
 */
bool	xmlrpc_client_t::notify_callback(const xmlrpc_err_t &xmlrpc_err
						, const datum_t &xmlrpc_resp)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_xmlrpc_client_cb(userptr, *this, xmlrpc_err, xmlrpc_resp);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



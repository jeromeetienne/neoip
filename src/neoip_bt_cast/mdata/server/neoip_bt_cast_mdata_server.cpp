/*! \file
    \brief Definition of the \ref bt_cast_mdata_server_t

\par Brief Description
This object is have a server to server bt_cast_mdata_t
*/

/* system include */
/* local include */
#include "neoip_bt_cast_mdata_server.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_xmlrpc_listener.hpp"
#include "neoip_xmlrpc_resp.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_cast_mdata_server_t::bt_cast_mdata_server_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_xmlrpc_listener	= NULL;
	m_xmlrpc_resp		= NULL;
}

/** \brief Destructor
 */
bt_cast_mdata_server_t::~bt_cast_mdata_server_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete xmlrpc_resp_t if needed
	nipmem_zdelete	m_xmlrpc_resp;
	// delete xmlrpc_listener_t if needed
	nipmem_zdelete	m_xmlrpc_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_cast_mdata_server_t::start(http_listener_t *m_http_listener
			, bt_cast_mdata_server_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;
	
	// build the http_uri_t on which to listen
	http_uri_t	xmlrpc_uri = "http://0.0.0.0/neoip_bt_cast_mdata_server_xmlrpc.cgi";
	http_uri_t	jsrest_uri = "http://0.0.0.0/neoip_bt_cast_mdata_server_jsrest.js";

	// start the xmlrpc_listener_t
	xmlrpc_err_t	xmlrpc_err;
	m_xmlrpc_listener= nipmem_new xmlrpc_listener_t();
	xmlrpc_err	= m_xmlrpc_listener->start(m_http_listener, xmlrpc_uri, jsrest_uri);
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);

	// start the xmlrpc_resp_t
	m_xmlrpc_resp	= nipmem_new xmlrpc_resp_t();
	xmlrpc_err	= m_xmlrpc_resp->start(m_xmlrpc_listener, this, NULL);
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);
	// add the method_name in the xmlrpc_resp_t
	m_xmlrpc_resp->insert_method("get_cast_mdata");

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xmlrpc_resp_t main callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref xmlrpc_resp_t when to notify an event
 * 
 * - WARNING: it is not allowed to destroy the xmlrpc_listener_t on which 
 *   whic xmlrpc_resp_t is listening on *DURING* the callback.
 * 
 * @return true if the xmlrpc_resp_t is still valid after the callback
 */
bool	bt_cast_mdata_server_t::neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &xmlrpc_err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw()
{
	// call the proper handler depending on the method_name
	if( method_name == "get_cast_mdata" ){
		xmlrpc_err_out	= xmlrpc_call_get_cast_mdata(xmlrpc_parse, xmlrpc_build);
	}else	DBG_ASSERT( 0 );

	// log to debug
	if( xmlrpc_err_out.failed() )	KLOG_ERR("xmlrpc_err_out=" << xmlrpc_err_out);
	// return tokeep	
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xmlrpc_resp_t call handler
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief handle the call for 'request_stream'
 */
xmlrpc_err_t	bt_cast_mdata_server_t::xmlrpc_call_get_cast_mdata(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw()
{
	xmlrpc_err_t	xmlrpc_err	= xmlrpc_err_t::OK;
	bt_cast_id_t	cast_id;
	bt_cast_mdata_t	cast_mdata;
	// log to debug
	KLOG_ERR("enter");
	// use xmlrpc_parse_t to get the arguments of the xmlrpc call
	NEOIP_XMLRPC_RESP_PARSE_BEGIN	(xmlrpc_parse);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, cast_id);
	NEOIP_XMLRPC_RESP_PARSE_END	(xmlrpc_parse, xmlrpc_err);
	// if there is a error in the xmlrpc_parse_t, return now
	if( xmlrpc_err.failed() )	return xmlrpc_err;

	// use the callback to get the bt_cast_mdata_t for this cast_name
	bool	tokeep	= notify_callback(cast_id, &cast_mdata);
	DBG_ASSERT( tokeep == true );	// NOTE: in fact it has tokeep for regularity only :)
	if( !tokeep )	return xmlrpc_err;

	// if none matches, return an error
	if( cast_mdata.is_null() )
		return xmlrpc_err_t(xmlrpc_err_t::ERROR, "Unknown cast_id ("+cast_id.to_string()+")");

	// build the xmlrpc response with the bt_cast_mdata_t
	NEOIP_XMLRPC_RESP_BUILD(xmlrpc_build, cast_mdata );

	// return noerror
	return	xmlrpc_err_t::OK;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the bt_err_t
 */
bool bt_cast_mdata_server_t::notify_callback(const bt_cast_id_t &cast_id
					, bt_cast_mdata_t *cast_mdata_out)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_cast_mdata_server_cb(userptr, *this, cast_id
								, cast_mdata_out);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}
NEOIP_NAMESPACE_END;





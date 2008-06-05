/*! \file
    \brief Declaration of the bt_cast_mdata_client_t

\par Brief Description
\ref bt_cast_mdata_client_t does a single xmlrpc call to a xmlrpc server.
like the one implemented in bt_cast_mdata_server_t and return 
a bt_cast_mdata_t.

*/

/* system include */
/* local include */
#include "neoip_bt_cast_mdata_client.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_xmlrpc_client.hpp"
#include "neoip_xmlrpc_client_helper.hpp"
#include "neoip_xmlrpc_err.hpp"
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
bt_cast_mdata_client_t::bt_cast_mdata_client_t()		throw()
{
	// zero some fields
	xmlrpc_client	= NULL;
}

/** \brief Desstructor
 */
bt_cast_mdata_client_t::~bt_cast_mdata_client_t()		throw()
{
	// destroy the xmlrpc_client_t if needed
	nipmem_zdelete	xmlrpc_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief start function - the generic one used by all the helper one
 */
bt_err_t bt_cast_mdata_client_t::start_common(const http_uri_t &m_server_uri, const datum_t &xmlrpc_call
			, bt_cast_mdata_client_cb_t *callback, void * userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter server_uri=" << m_server_uri);
	// copy the parameter
	this->m_server_uri	= m_server_uri;
	this->callback		= callback;
	this->userptr		= userptr;

	// start the xmlrpc_sclient_t
	xmlrpc_err_t	xmlrpc_err;
	xmlrpc_client	= nipmem_new xmlrpc_client_t();
	xmlrpc_err	= xmlrpc_client->start(server_uri(), xmlrpc_call, this, NULL);
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);

	// return no error
	return bt_err_t::OK;
}

/** \brief start function - the generic one used by all the helper one
 */
bt_err_t bt_cast_mdata_client_t::start(const http_uri_t &m_server_uri
			, const std::string &m_cast_name, const std::string &m_cast_privhash
			, bt_cast_mdata_client_cb_t *callback, void * userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter server_uri=" << m_server_uri << " cast_name="<< m_cast_name
						<< " cast_privhash=" << m_cast_privhash);

	// build the call_datum
	xmlrpc_build_t	xmlrpc_build;
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("get_cast_mdata");
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG << m_cast_name	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG << m_cast_privhash	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build << xmlrpc_build_t::CALL_END;
	// get the datum_t containing the just built xmlrpc call
	datum_t		xmlrpc_call;
	xmlrpc_call	= xmlrpc_build.to_datum();
	// forward to ::start_common
	return start_common(m_server_uri, xmlrpc_call, callback, userptr);
}


/** \brief start function - the generic one used by all the helper one
 */
bt_err_t bt_cast_mdata_client_t::start(const http_uri_t &m_server_uri
			, const bt_cast_id_t &m_cast_id
			, bt_cast_mdata_client_cb_t *callback, void * userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter server_uri=" << m_server_uri << " cast_id="<< m_cast_id);

	// build the call_datum
	xmlrpc_build_t	xmlrpc_build;
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("get_cast_mdata");
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG << m_cast_id	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build << xmlrpc_build_t::CALL_END;
	// get the datum_t containing the just built xmlrpc call
	datum_t		xmlrpc_call;
	KLOG_ERR("xmlrpc=" << xmlrpc_build.to_datum());
	xmlrpc_call	= xmlrpc_build.to_datum();	

	// forward to ::start_common
	return start_common(m_server_uri, xmlrpc_call, callback, userptr);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     xmlrpc_client_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notifying the result of xmlrpc_client_t
 */
bool	bt_cast_mdata_client_t::neoip_xmlrpc_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &cb_xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw()
{
	xmlrpc_err_t	xmlrpc_err;
	// log to debug
	KLOG_ERR("enter xml_datum=" << xmlrpc_resp.to_stdstring());
	// test if the xmlrpc_client_t succeed
	xmlrpc_err	= xmlrpc_client_helper_t::has_succeed(cb_xmlrpc_err, xmlrpc_resp);
	if( xmlrpc_err.failed() ){
		bt_err_t	bt_err	= bt_err_from_xmlrpc(xmlrpc_err);
		return notify_callback_failure(bt_err);
	}	
	
	// parse the xmlrpc response
	bt_cast_mdata_t	ret_value;
	try {
		// set the document for xmlrpc_parse_t
		xmlrpc_parse_t	xmlrpc_parse;
		xmlrpc_parse.set_document(xmlrpc_resp);
		// passwd the reply
		xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
		xmlrpc_parse	>> ret_value;
		xmlrpc_parse >> xmlrpc_parse_t::RESP_END;
	} catch(xml_except_t &e) {
		std::string reason	= "unable to parse response as bt_cast_mdata_t due to " + e.what();
		return	notify_callback_failure(bt_err_t(bt_err_t::ERROR, reason));
	}
	
	// notify the caller
	return notify_callback_success(ret_value);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Notify the callback when it is success
 */
bool bt_cast_mdata_client_t::notify_callback_success(const bt_cast_mdata_t &ret_value)	throw()
{
	return notify_callback(bt_err_t::OK, ret_value);
}

/** \brief Notify the callback when it is failure
 */
bool bt_cast_mdata_client_t::notify_callback_failure(const bt_err_t &bt_err)	throw()
{
	return notify_callback(bt_err, bt_cast_mdata_t());
}

/** \brief notify the callback
 */
bool bt_cast_mdata_client_t::notify_callback(const bt_err_t &bt_err
					, const bt_cast_mdata_t &ret_value)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_cast_mdata_client_cb(userptr, *this, bt_err, ret_value);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



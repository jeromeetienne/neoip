/*! \file
    \brief Definition of the \ref bt_cast_mdata_unpublish_t
    
\par Brief Description
bt_cast_mdata_unpublish_t unpublish a bt_cast_mdata_t in a bt_cast_server_t
  
*/

/* system include */
/* local include */
#include "neoip_bt_cast_mdata_unpublish.hpp"
#include "neoip_xmlrpc_client.hpp"
#include "neoip_xmlrpc_err.hpp"

#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_cast_mdata_unpublish_t::bt_cast_mdata_unpublish_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_xmlrpc_client	= NULL;
}

/** \brief Destructor
 */
bt_cast_mdata_unpublish_t::~bt_cast_mdata_unpublish_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the xmlrpc_client if needed
	nipmem_zdelete	m_xmlrpc_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 * 
 * - NOTE: just copy the data, the publication is not started 
 *   before bt_cast_mdata_unpublish_t::start_unpublishing() is called 
 */
bt_err_t bt_cast_mdata_unpublish_t::start(const http_uri_t &mdata_srv_uri
				, const std::string &cast_name, const std::string &cast_privtext
				, bt_cast_mdata_unpublish_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;
	
	// build the call_datum
	xmlrpc_build_t	xmlrpc_build;
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("del_cast_mdata");
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG << cast_name	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG << cast_privtext	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build << xmlrpc_build_t::CALL_END;
	// get the datum_t containing the just built xmlrpc call
	datum_t		xmlrpc_call;
	xmlrpc_call	= xmlrpc_build.to_datum();	

	// start the xmlrpc_sclient_t
	xmlrpc_err_t	xmlrpc_err;
	m_xmlrpc_client	= nipmem_new xmlrpc_client_t();
	xmlrpc_err	= m_xmlrpc_client->start(mdata_srv_uri, xmlrpc_call, this, NULL);
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);

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
bool	bt_cast_mdata_unpublish_t::neoip_xmlrpc_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw()
{
	bt_err_t	bt_err;
	// process the xmlrpc_resp
	bt_err	= handle_xmlrpc_resp(xmlrpc_err, xmlrpc_resp);
	// notify the caller of the completion
	return notify_callback(bt_err);
}

/** \brief Handle the xmlrpc_client_t response
 */
bt_err_t	bt_cast_mdata_unpublish_t::handle_xmlrpc_resp(const xmlrpc_err_t &xmlrpc_err
						, const datum_t &xmlrpc_resp)	throw()
{
	// log to debug
	KLOG_DBG("enter xmlrpc_err=" << xmlrpc_err << " xmlrpc_resp=" << xmlrpc_resp.to_stdstring());
	// handle the error case
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);
	
	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_resp);
	// if it is impossible to parse the xml, return an error
	if( xmlrpc_parse.is_null() )	return bt_err_t(bt_err_t::ERROR, "unable to parse the xml");
	
	// handle the fault case
	if( xmlrpc_parse.is_fault_resp() ){
		int32_t		fault_code;
		std::string	fault_string;
		try {
			xmlrpc_parse >> xmlrpc_parse_t::FAULT(fault_code, fault_string);
		} catch(xml_except_t &e) {
			return bt_err_t(bt_err_t::ERROR, "unable to parse the xmlrpc fault");
		}
		std::string reason	= "XMLRPC fault code="	+ OSTREAMSTR(fault_code)
								+ " reason=" + fault_string;
		return bt_err_t(bt_err_t::ERROR, reason);
	}

	// parse the xmlrpc response
	int	status_code;
	try {
		xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
		xmlrpc_parse	>> status_code;
		xmlrpc_parse >> xmlrpc_parse_t::RESP_END;
	} catch(xml_except_t &e) {
		return bt_err_t(bt_err_t::ERROR, "unable to parse response as int due to " + e.what());
	}
	// log to debug
	KLOG_DBG("status_code=" << status_code);
	// if status_code is not 0, return an error
	if( status_code )
		return bt_err_t(bt_err_t::ERROR, "server returned a status_code " + OSTREAMSTR(status_code));
	
	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the bt_err_t
 */
bool bt_cast_mdata_unpublish_t::notify_callback(const bt_err_t &bt_err)		throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_cast_mdata_unpublish_cb(userptr, *this, bt_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;





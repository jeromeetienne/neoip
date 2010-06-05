/*! \file
    \brief Definition of the \ref xmlrpc_listener_t

\par Brief Description
xmlrpc_listener_t is listener for xmlrpc call. Each method_name is registered 
by creating a xmlrpc_resp_t with it.

- TODO there is a json-rpc going aroung on the web
  - jsrest is only a custom stuff without any specific
  - this is only because i needed something and i didnt know about json-rpc
  - maybe i could port it to this more standard one

\par About double support xmlrpc and jsrest
- xmlrpc_listener_t and xmlrpc_resp_t provides a single api for both rpc 
  system. xmlrpc and jsrest
- jsrest is done by converting the call and response to/from xmlrpc.
  - TODO currently the jsrest has limitations
    - the call can only specify variables of type std::string
    - the returned value is currently only a uint32_t(0)
      - the one returned by the function is ignored
  
\par About jsrest
- jsrest is a special rpc system specifically designed to be used from web browser
- javascript has an XmlHttpRequest object. but it got a feature/bug which prevent
  it from calling other hosts beside the one of the browser location bar.
- this dramatically reduces the usefullness of the xmlrpc
- so everybody is using some kind of workaround this bug/feature
- the one implemented here is using <script> because it workaround the 
  crossdomain limitation.

\par ISSUE with jsrest
- how to get the type of the argument with jsrest
  - currently i only need string... do i get away with only this for now ?
  - possible to workaround if variable like type0/type1 are used
- DONE how to return json stuff from the xmlrpc_built_t ?
  - do i do it inside neoip or in the javascript ?
  - javascript is slow
  - currently i dont need any data in return
    - i could get away with only returning 0...
- those 2 issues may left remaining for now
  - but this create an half backed implementation
  - i dont like that.
  - on the other hand, i got MANY thing in progress now...
  
*/

/* system include */
/* local include */
#include "neoip_xmlrpc_listener.hpp"
#include "neoip_xmlrpc_resp.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_http_sresp.hpp"
#include "neoip_http_sresp_ctx.hpp"
#include "neoip_http_resp_mode.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
xmlrpc_listener_t::xmlrpc_listener_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some variables
	m_http_listener	= NULL;
	m_sresp_xmlrpc	= NULL;
	m_sresp_jsrest	= NULL;
}

/** \brief Destructor
 */
xmlrpc_listener_t::~xmlrpc_listener_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// close all pending xmlrpc_resp_t
	while( !resp_db.empty() )	nipmem_delete resp_db.front();
	// delete m_sresp_xmlrpc if needed
	nipmem_zdelete	m_sresp_xmlrpc;
	// delete m_sresp_jsrest if needed
	nipmem_zdelete	m_sresp_jsrest;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 * 
 * - at lease one of the xmlrpc_uri and jsrest_uri MUST be set
 * 
 * \param xmlrpc_uri	the http_uri_t on which to listen for the xmlrpc call.
 *			If null, dont listen to xmlrpc
 * \param jsrest_uri	the http_uri_t on which to listen for the jsrest call
 *			If null, dont listen to jsrest
 */
xmlrpc_err_t	xmlrpc_listener_t::start(http_listener_t *m_http_listener
			, const http_uri_t &xmlrpc_uri, const http_uri_t &jsrest_uri)	throw()
{
	http_err_t	http_err; 
	// log to debug
	KLOG_DBG("enter");
	// sanity check - at lease one of the xmlrpc_uri and jsrest_uri MUST be set
	DBG_ASSERT( !xmlrpc_uri.is_null() || !jsrest_uri.is_null() ); 

	// copy the parameter
	this->m_http_listener	= m_http_listener;
		
	// start the m_sresp_xmlrpc
	if( !xmlrpc_uri.is_null() ){
		m_sresp_xmlrpc	= nipmem_new http_sresp_t();
		http_err	= m_sresp_xmlrpc->start(http_listener(), xmlrpc_uri
					, http_method_t::POST, http_resp_mode_t::REJECT_SUBPATH
					, this, NULL);
		if( http_err.failed() )	return xmlrpc_err_from_http(http_err);
	}

	// start the m_sresp_jsrestm_sresp_jsrest
	if( !jsrest_uri.is_null() ){
		m_sresp_jsrest	= nipmem_new http_sresp_t();
		http_err	= m_sresp_jsrest->start(http_listener(), jsrest_uri
					, http_method_t::GET, http_resp_mode_t::REJECT_SUBPATH
					, this, NULL);
		if( http_err.failed() )	return xmlrpc_err_from_http(http_err);
	}	
	// return no error
	return xmlrpc_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the listen_uri for the xmlrpc itself
 */
const http_uri_t &	xmlrpc_listener_t::xmlrpc_uri()	const throw()
{
	return m_sresp_xmlrpc->listen_uri();
}

/** \brief Return the listen_uri for the jsrest itself
 */
const http_uri_t &	xmlrpc_listener_t::jsrest_uri()	const throw()
{
	return m_sresp_jsrest->listen_uri();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Return a pointer on a xmlrpc_resp_t which match this http_uri_t
 */
xmlrpc_resp_t * xmlrpc_listener_t::resp_by_method_name(const std::string &method_name)
										const throw()
{
	std::list<xmlrpc_resp_t *>::const_iterator	iter;
	// go thru the whole resp_db
	for(iter = resp_db.begin(); iter != resp_db.end(); iter++){
		xmlrpc_resp_t *	xmlrpc_resp	= *iter;
		// if this xmlrpc_resp_t matches this method_name, return its pointer
		if( xmlrpc_resp->contain_method(method_name) )	return xmlrpc_resp;
	}
	// if this point is reached, no matches have been found
	return NULL;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			http_sresp_t main callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_resp_t when to notify an event
 * 
 * @return true if the http_resp_t is still valid after the callback
 */
bool	xmlrpc_listener_t::neoip_http_sresp_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
				, http_sresp_ctx_t &sresp_ctx)		throw()
{
	// forward the callback to the proper handler
	if( &cb_http_sresp == m_sresp_xmlrpc )	return sresp_xmlrpc_cb(cb_userptr,cb_http_sresp, sresp_ctx);
	if( &cb_http_sresp == m_sresp_jsrest )	return sresp_jsrest_cb(cb_userptr,cb_http_sresp, sresp_ctx);
	// NOTE: this point SHOULD NEVER be reached
	DBG_ASSERT( 0 );
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			m_sresp_xmlrpc callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_resp_t when to notify an event
 * 
 * @return true if the http_resp_t is still valid after the callback
 */
bool	xmlrpc_listener_t::sresp_xmlrpc_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
						, http_sresp_ctx_t &sresp_ctx)	throw()
{
	xmlrpc_err_t	xmlrpc_err;
	// log to debug
	KLOG_DBG("enter sresp_ctx=" << sresp_ctx);
	// get the xmlenc_datum
	datum_t		xmlenc_datum	= sresp_ctx.post_data().to_datum(datum_t::NOCOPY);
	// parse the xmlrpc_call
	std::string	resp_str	= parse_xmlrpc_call(xmlenc_datum);
	// log to debug
	KLOG_DBG("resp_str=" << resp_str);


	// Set the mime-type for xml
	sresp_ctx.rephd().header_db().update("Content-Type", "text/xml");
	// put the xmlrpc response into the sresp_ctx.response_body
	sresp_ctx.response_body() << resp_str;
	// return tokeep
	return true;
}


/** \brief Parse the xmlrpc_call
 */
std::string xmlrpc_listener_t::parse_xmlrpc_call(const datum_t &xmlenc_datum)	throw()
{
	xmlrpc_parse_t	xmlrpc_parse;
	// set the document for xmlrpc_parse_t
	xmlrpc_parse.set_document(xmlenc_datum);
	// if cant parse the xml, return an error
	if( xmlrpc_parse.is_null() ){
		xmlrpc_build_t	xmlrpc_build;
		xmlrpc_build << xmlrpc_build_t::FAULT(-1, "unable to parse the xml");
		return xmlrpc_build.to_stdstring();
	}

	// get the method_name
	std::string	method_name;	
	try {
		xmlrpc_parse >> xmlrpc_parse_t::CALL_BEG(method_name);
	} catch(xml_except_t &e){
		KLOG_ERR("xml_except_t=" << e.what());
		xmlrpc_build_t	xmlrpc_build;
		xmlrpc_build << xmlrpc_build_t::FAULT(-1, "unable to parse the method_name");
		return xmlrpc_build.to_stdstring();
	}

	// try to get a xmlrpc_resp_t for this method_name
	xmlrpc_resp_t *	xmlrpc_resp;
	xmlrpc_resp	= resp_by_method_name(method_name);

	// if no xmlrpc_resp_t matches this method_name, return an error
	if( !xmlrpc_resp ){
		xmlrpc_build_t	xmlrpc_build;
		xmlrpc_build << xmlrpc_build_t::FAULT(-1, "unable to find the method_name");
		return xmlrpc_build.to_stdstring();
	}
		
	// declare some variables
	xmlrpc_build_t	xmlrpc_build;
	xmlrpc_err_t	xmlrpc_err;

	// notify the xmlrpc_resp_t callback
	// - NOTE: it is not allowed to destroy the xmlrpc_listener_t DURING the callback
	bool	tokeep	= xmlrpc_resp->notify_callback(method_name, xmlrpc_err
						, xmlrpc_parse, xmlrpc_build);

	// if the callback return a xmlrpc_err_t, notify a xmlrpc FAULT
	if( xmlrpc_err.failed() ){
		// sanity check - xmlrpc_build MUST NOT be set if xmlrpc_err is set
		DBG_ASSERT( xmlrpc_build.is_null() );
		xmlrpc_build << xmlrpc_build_t::FAULT(-1, xmlrpc_err.get_reason());
	}
	
	// if xmlrpc_build is null and xmlrpc_resp_t dontkeep, return internal error
	// - NOTE: this means the xmlrpc_resp_t has been deleted before 
	//   providing a xmlrpc_build_t answer
	if( xmlrpc_build.is_null() && !tokeep )
		xmlrpc_build << xmlrpc_build_t::FAULT(-1, "internal error");

	// sanity check - at this point, xmlrpc_build MUST NOT be null
	DBG_ASSERT( !xmlrpc_build.is_null() );

	// return the xmlrpc_build_t into a std::string
	return xmlrpc_build.to_stdstring();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			m_sresp_jsrest callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_resp_t when to notify an event
 * 
 * @return true if the http_resp_t is still valid after the callback
 */
bool	xmlrpc_listener_t::sresp_jsrest_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
						, http_sresp_ctx_t &sresp_ctx)	throw()
{
	const strvar_db_t &	uri_var		= sresp_ctx.reqhd().uri().var();
	const std::string &	obj_id		= uri_var.get_first_value("obj_id"); 
	const std::string &	jsonp_cb	= uri_var.get_first_value("callback"); 
	const std::string &	method_name	= uri_var.get_first_value("method_name"); 
	// log to debug
	KLOG_ERR("enter sresp_ctx=" << sresp_ctx);
	
	// if any mandatory field is not specified, return a "bad request"
	if( method_name.empty() ){
		sresp_ctx.reply_error(400, "Invalid request. missing mandatory field obj_id/method_name");
		return true;
	}

	// start to build the xmlrpc call for the emulation
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::CALL_BEG(method_name);
	// go thru the all the argument of the uri_var
	// - NOTE: 99 is a dummy number, large enougth. more than 99 arg is a lot :)   
	for(size_t i = 0; i < 99; i++ ){
		std::string	arg_key	= "arg" + OSTREAMSTR(i);
		// if this arg_key is not contained, leave the loop
		if( !uri_var.contain_key(arg_key) )	break;
		// put this value into the xmlrpc_build
		// - NOTE: this is ALWAYS a string....
		// - aka impossible to pass other variable type beside std::string
		// - see the header of this file for more comment
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG;
		xmlrpc_build	<< uri_var.get_first_value(arg_key);
		xmlrpc_build << xmlrpc_build_t::PARAM_END;
	}	
	// end the xmlrpc call building
	xmlrpc_build << xmlrpc_build_t::CALL_END;
	
	// log to debug
	KLOG_DBG("build=" << xmlrpc_build.to_stdstring());

	// get the xmlenc_datum
	datum_t		xmlenc_datum	= xmlrpc_build.to_datum();
	// parse the xmlrpc_call 
	std::string	resp_str	= parse_xmlrpc_call(xmlenc_datum);

	
	/*************** Convert the resp_str from xmlrpc to json	*******/
	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(datum_t(resp_str));
	DBG_ASSERT( !xmlrpc_parse.is_null() );

	// start producing the javascript to return
	std::ostringstream	js_oss;
	if( !obj_id.empty() ){
		// jsreset
		#if 0	// TODO to remove - old version i dunno what window.parent was used for
			// - likely for experiement with iframe
			js_oss	<< "window.parent.neoip_xdomrpc_script_reply_var_" << obj_id << " = ";
		#else
			js_oss	<< "neoip_xdomrpc_script_reply_var_" << obj_id << " = ";
		#endif
	}else if( !jsonp_cb.empty() ){
		// jsonp
		js_oss	<< jsonp_cb << "(";
	}else {
		// plain reset
	}

	// build the reply depending on the xmlrpc_parse_t
	if( xmlrpc_parse.is_fault_resp() ){
		int32_t		fault_code;
		std::string	fault_string;
		// get the xmlrpc FAULT code/string
		try {	xmlrpc_parse >> xmlrpc_parse_t::FAULT(fault_code, fault_string);
		} catch(xml_except_t &e) { DBG_ASSERT(0);	}
		// log to debug
		KLOG_ERR("fault_code=" << fault_code << " fault_string=" << fault_string);
		// complete the js_oss
		js_oss << "{";
		js_oss 	<< "\"fault\": ";
		js_oss 		<< "{\"code\": "	<< fault_code; 
		js_oss		<< ", \"string\": \""	<< fault_string << "\"";
		js_oss		<< "}";
		js_oss << ", ";
		js_oss 	<< "\"returned_val\": ";
		js_oss		<< "null";
		js_oss << "}";
	}else{
		// log to debug
		KLOG_ERR("succeed");
		// get the xmlrpc value
		std::ostringstream	oss_json;
		try {
			xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
			xmlrpc_parse	>> xmlrpc_parse_t::VALUE_TO_JSON(oss_json);
			xmlrpc_parse >> xmlrpc_parse_t::RESP_END;		
		} catch(xml_except_t &e) { DBG_ASSERT(0);	}
		// complete the js_oss
		js_oss << "{";
		js_oss 	<< "\"fault\": ";
		js_oss		<< "null";
		js_oss << ", ";
		js_oss 	<< "\"returned_val\": ";
		js_oss		<< oss_json.str();
		js_oss << "}";
	}

	// Set the mime-type for xml
	sresp_ctx.rephd().header_db().update("Content-Type", "application/x-javascript");
	// build the html reply
	std::ostringstream &	oss	= sresp_ctx.response_body();
	if( !jsonp_cb.empty() )	js_oss	<< ")";
	oss	<< js_oss.str() << ";\n";

	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END;





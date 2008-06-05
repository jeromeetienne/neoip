/*! \file
    \brief Declaration of the upnp_call_t

\par Brief Description
\ref upnp_call_t does a single SOAP call.
This handles any type of SOAP call (well the upnp subset) but there are some
helper start function to help calling the functions hiding the upnp stuff.

\par About http_sclient_t cnx_err and upnp router 'overload'
- some upnp routers accepts only a *very* limited number of simutaneous connections
  - the adsl box from 'neuf telecom', aka the 'neuf box', accept only 1 at the time
- some upnp routers accept (as in bsd socket API) the connection but close it immediatly
  - to handle this case, if the http_sclient_t receive a cnx_err, it will retry several
    time to make it more robust
- the retry is done by a typical randomized exponantial timer with a maximum
  amount of retry.

*/

/* system include */
/* local include */
#include "neoip_upnp_call.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_nudp.hpp"
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
upnp_call_t::upnp_call_t()		throw()
{
	// zero some fields
	http_sclient		= NULL;
	httperr_retry_nbcur	= 0;
}

/** \brief Desstructor
 */
upnp_call_t::~upnp_call_t()		throw()
{
	// destroy the http_sclient_t if needed
	nipmem_zdelete	http_sclient;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
upnp_call_t &upnp_call_t::set_profile(const upnp_call_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == upnp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief start function - the generic one used by all the helper one
 */
upnp_err_t upnp_call_t::start(const http_uri_t &m_control_uri, const std::string &m_service_name
				, const std::string &m_method_name
				, const strvar_db_t &m_strvar_db, upnp_call_cb_t *callback
				, void * userptr)				throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the parameter MUST be set
	DBG_ASSERT( m_control_uri.is_null()	== false);
	DBG_ASSERT( m_service_name.empty()	== false);
	DBG_ASSERT( m_method_name.empty()	== false);
	// copy the parameter
	this->m_control_uri	= m_control_uri;
	this->m_service_name	= m_service_name;
	this->m_method_name	= m_method_name;
	this->m_strvar_db	= m_strvar_db;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// init the itor_expire_timeout
	expire_timeout.start(profile.expire_delay(), this, NULL);

	// launch the http_sclient_t
	upnp_err_t	upnp_err;
	upnp_err	= launch_http_sclient();
	if( upnp_err.failed() )	return upnp_err;

	// return no error
	return upnp_err_t::OK;
}

/** \brief Launch the http_sclient_t
 */
upnp_err_t	upnp_call_t::launch_http_sclient()				throw()
{
	// sanity check - http_sclient_t MUST be NULL
	DBG_ASSERT( !http_sclient );
	
	// build the data2post
	datum_t		data2post;
	data2post	= build_soap_req(service_name(), method_name(), strvar_db()).to_datum();
	// build the http_reqhd_t to use for the http_sclient_t
	http_reqhd_t	http_reqhd;
	http_reqhd	= build_soap_reqhd(control_uri(), service_name(), method_name(), data2post);

	// log to debug
	KLOG_DBG("http_reqhd="	<< http_reqhd);
	KLOG_DBG("data2post="	<< data2post.to_stdstring());

	// start the http_sclient_t
	http_err_t	http_err;
	http_sclient	= nipmem_new http_sclient_t();
	http_err	= http_sclient->start(http_reqhd, this, NULL, data2post);
	if( http_err.failed() )	return upnp_err_from_http(http_err);

	// return no error
	return upnp_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			build function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a http_reqhd_t for soap
 */
http_reqhd_t	upnp_call_t::build_soap_reqhd(const http_uri_t &control_uri
				, const std::string &service_name, const std::string &method_name
				, const datum_t &data2post)	const throw()
{
	http_reqhd_t	http_reqhd;
	// build the http_reqhd_t to use for the http_sclient_t
	http_reqhd.method("POST").uri(control_uri);
	http_reqhd.header_db().append("CONTENT-LENGTH", OSTREAMSTR(data2post.length()));
	http_reqhd.header_db().append("CONTENT-TYPE", "text/xml; charset=\"utf-8\"");
	http_reqhd.header_db().append("SOAPACTION", std::string("\"urn:schemas-upnp-org:service:")
					+ service_name + ":1#" + method_name + "\"");
	// return the just built http_reqhd_t
	return http_reqhd;	
}

/** \brief Build a soap request
 */
pkt_t	upnp_call_t::build_soap_req(const std::string &service_name, const std::string &method_name
						, const strvar_db_t &strvar_db)	const throw()
{
	std::ostringstream	oss;
	// build the payload header
	oss << "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">";
	oss << "<s:Body>";
	oss	<<"<u:" << method_name;
	oss		<< " xmlns:u=\"urn:schemas-upnp-org:service:" << service_name << ":1\">";
	// add all the variables in strvar_db_t
	for(size_t i = 0; i < strvar_db.size(); i++){
		const strvar_item_t &	variable	= strvar_db[i]; 
		oss << "<" + variable.key() + ">";
		oss	<< variable.val();
		oss << "</" + variable.key() + ">";
	}
	// build the payload footer
	oss	<<"</u:" << method_name << ">";
	oss << "</s:Body>";
	oss << "</s:Envelope>";
	// return the pkt_t for it
	return pkt_t(oss.str().c_str(), oss.str().size());	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parse function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse a soap reply
 */
upnp_err_t upnp_call_t::parse_soap_rep(const datum_t &xml_datum, strvar_db_t &strvar_db)	const throw()
{
	// log to debug
	KLOG_DBG("enter xml_datum=" << xml_datum.to_stdstring());
	// set the document to parse
	xml_parse_doc_t	xml_parse_doc;
	xml_parse_doc.set_document(xml_datum);
	// if it is impossible to parse the xml, return an error
	if( xml_parse_doc.is_null() ){
		KLOG_ERR("unable to parse the xml in " << xml_datum);
		return upnp_err_t(upnp_err_t::ERROR, "unable to parse the xml");
	}

	// init the xml_parse_t on this document
	xml_parse_t	xml_parse(&xml_parse_doc);
	// go inside the "Body"
	try {
		xml_parse.goto_children().goto_firstsib("Body").goto_children();
	}catch(xml_except_t &e){
		KLOG_ERR("unable to parse the xml 'subbody' due to " << e.what() << " in " << xml_datum);
		return upnp_err_t(upnp_err_t::ERROR, "unable to parse the xml 'subbody' due to " + e.what());
	}

	// if this is a SOAP_FAULT, goto the fault parser
	if( xml_parse.has_firstsib("Fault") )
		return parse_soap_rep_fault(xml_datum, xml_parse, strvar_db);
	
	// else goto the success parser
	return parse_soap_rep_success(xml_datum, xml_parse, strvar_db);
}

/** \brief Parse a soap reply in case of a success
 */
upnp_err_t upnp_call_t::parse_soap_rep_success(const datum_t &xml_datum, xml_parse_t &xml_parse	
							, strvar_db_t &strvar_db)	const throw()
{
	// log to debug
	KLOG_DBG("enter in " << xml_datum.to_stdstring());
	// parse all the variables contained in the Response
	try {
		xml_parse.goto_firstsib(method_name() + "Response");
		// if the response MAY not have any children is there are no variable returned
		bool	has_variable	= xml_parse.has_children();	
		// if there are variable, goto_children() where the variable are 
		if( has_variable )	xml_parse.goto_children();
		// go thru all the sibling at this level - which reprensent the variable
		while( has_variable ){
			// log to debug
			KLOG_DBG("nodename=" << xml_parse.node_name());
			KLOG_DBG("nodecontent=" << xml_parse.node_content());
			// if this node_name is NOT "text", insert it in the strvar_db_t
			// - NOTE: "text" is the nodename used by the xml parser for the \r\n inserted
			//   between the xml node. so they are to skip 
			if( xml_parse.node_name() != "text" ){
				strvar_db.append(xml_parse.node_name(), xml_parse.node_content());
			}
			// if there are nomore sibling, leave the loop
			if( !xml_parse.has_nextsib() )	break;
			// goto the next sibling
			xml_parse.goto_nextsib();
		};
	}catch(xml_except_t &e){
		KLOG_ERR("unable to parse the xml 'response variable' due to " << e.what() << " in " << xml_datum.to_stdstring());
		return upnp_err_t(upnp_err_t::ERROR, "unable to parse the xml 'response varialble' due to " + e.what());
	}
	
	// return no error
	return upnp_err_t::OK;
}

/** \brief Parse a soap reply in case of a fault
 */
upnp_err_t upnp_call_t::parse_soap_rep_fault(const datum_t &xml_datum, xml_parse_t &xml_parse
							, strvar_db_t &strvar_db)	const throw()
{
	std::string	reason	= "FAULT";
	// parse all a bit of the variables contained in the Response
	try {
		xml_parse.goto_children().goto_firstsib("detail");
		xml_parse.goto_children().goto_children();
		// go thru all the sibling at this level
		while( true ){
			// log to debug
			KLOG_DBG("nodename=" << xml_parse.node_name());
			KLOG_DBG("nodecontent=" << xml_parse.node_content());
			// if this node_name is NOT "text", insert it in the strvar_db_t
			// - NOTE: "text" is the nodename used by the xml parser for the \r\n inserted
			//   between the xml node. so they are to skip 
			if( xml_parse.node_name() != "text" ){
				reason += std::string(" ") +xml_parse.node_name() + "='" + xml_parse.node_content() + "'";
			}
			// if there are nomore sibling, leave the loop
			if( !xml_parse.has_nextsib() )	break;
			// goto the next sibling
			xml_parse.goto_nextsib();
		};
	}catch(xml_except_t &e){
		KLOG_ERR("unable to parse the xml 'response variable' due to " << e.what() << " in " << xml_datum);
		// just fall thru and return a SOAP_FAULT 
	}
	// return a SOAP_FAULT to notify to the caller that the upnp_call_t returned a fault
	return upnp_err_t(upnp_err_t::SOAP_FAULT, reason);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	upnp_call_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &cb_sclient_res)	throw() 
{
	http_sclient_res_t	sclient_res	= cb_sclient_res;
	// log to debug
	KLOG_DBG("enter http_sclient_res=" << sclient_res);
	KLOG_DBG("reply_body=" << sclient_res.reply_body().to_stdstring());
	
	// if http_sclient_t failed, handle the httperr_retry
	// - NOTE: it is pressuposed to be the server dicarding http request 
	//   because of 'overload'.
	// - some upnp router support only a single http request at a time. so 
	//   they are frequently 'overloaded'
	if( !sclient_res.is_post_ok() ){
		//KLOG_ERR("enter http_sclient_res=" << sclient_res);
		//KLOG_ERR("reply_body=" << sclient_res.reply_body().to_stdstring());
		return handle_http_sclient_error();
	}

	// get the reply_body which is in xml
	std::string	xml_str	= sclient_res.reply_body().to_stdstring();
	// some router get the idea it is good to put additionnal \r\n, so i strip them :)
	xml_str		= string_t::strip(xml_str, "\r\n \t");
	// parse the soap reply
	strvar_db_t	strvar_db;
	upnp_err_t	upnp_err;
	upnp_err	= parse_soap_rep(datum_t(xml_str), strvar_db);
	
	// notify the caller
	return notify_callback(upnp_err, strvar_db);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when one of the timeout_t expire
 */
bool upnp_call_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// just forward to the proper handlers
	if( &expire_timeout == &cb_timeout )		return expire_timeout_cb(userptr, cb_timeout);
	if( &httperr_retry_timeout == &cb_timeout )	return httperr_retry_timeout_cb(userptr, cb_timeout);
	// NOTE: this point MUST NOT be reached
	DBG_ASSERT( 0 );
	return true;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			expire_timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_call_t::expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// notify the caller of the expiration
	std::string reason = "upnp_call_t timedout after " + OSTREAMSTR(expire_timeout.get_period());
	return notify_callback(upnp_err_t(upnp_err_t::ERROR, reason), strvar_db_t());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			httperr_retry
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief handle the connection error for http_sclient_t
 * 
 * @return a tokeep for the http_sclient
 */
bool	upnp_call_t::handle_http_sclient_error()	throw()
{
	// log to debug
	KLOG_DBG("enter httperr_retry_nbcur=" << httperr_retry_nbcur);
	// delete the http_sclient_t
	nipmem_zdelete	http_sclient;

	// init the httperr_retry_delay, if it is the first retry	
	if( httperr_retry_nbcur == 0 )
		httperr_retry_delaygen	= profile.httperr_retry_delaygen();
	
	// if the maximum number httperr_retry already occured, notify an error
	if( httperr_retry_nbcur == profile.httperr_retry_nbmax() ){
		std::string reason = "Retried " + OSTREAMSTR(profile.httperr_retry_nbmax()) + "-times due to cnxerr. giving up";
		// log the error
		KLOG_ERR(reason);
		// notify the caller
		notify_callback(upnp_err_t(upnp_err_t::ERROR, reason), strvar_db_t());
		return false;
	}

	// update the httperr_retry_nbcur
	httperr_retry_nbcur++;	
	
	// start the httperr_retry_timeout for the next_retry
	httperr_retry_timeout.start(httperr_retry_delaygen.post_inc(), this, NULL);
	
	// lot to debug
	KLOG_DBG("retry in "<< httperr_retry_timeout.get_period() );
	
	// return dontkeep - as the object has been deleted
	return false;
}

/** \brief callback called when the neoip_timeout expire
 */
bool upnp_call_t::httperr_retry_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter httperr_retry_nbcur=" << httperr_retry_nbcur);
	// sanity check - at this point, http_sclient MUST be NULL
	DBG_ASSERT( !http_sclient );

	// notify the expiration of the timeout to the delaygen
	httperr_retry_delaygen.notify_expiration();
	
	// stop the httperr_retry_timeout
	httperr_retry_timeout.stop();
	
	// relaunch a http_sclient_t
	upnp_err_t	upnp_err;
	upnp_err	= launch_http_sclient();
	if( upnp_err.failed() ){
		std::string	reason	= "impossible to launch http_sclient_t due to " + upnp_err.to_string();
		return notify_callback(upnp_err_t(upnp_err_t::ERROR, reason), strvar_db_t());
	}
	
	// return tokeey
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool upnp_call_t::notify_callback(const upnp_err_t &upnp_err, const strvar_db_t &strvar_db)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_upnp_call_cb(userptr, *this, upnp_err, strvar_db);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



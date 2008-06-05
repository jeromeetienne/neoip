/*! \file
    \brief Definition of the \ref bt_cast_prefetch_t

\par Brief Description
bt_cast_prefetch_t is service to allow webpage to create http connection 'internal'
to a single neoip-apps.
- It is done via xmlrpc
- it allow to create/destroy http client from this apps to this apps 
- those connections are toward http_uri_t with httpo_maxrate set to 0k
  - so the 'server' (neoip-casto/neoip-oload) wont deliver any data except 
    the http reply header
  - TODO this is a trivial DoS - make sanity check to ensure this  
- TODO comment more on this 
  - rather dirty/unusual
  
*/

/* system include */
/* local include */
#include "neoip_bt_cast_prefetch.hpp"
#include "neoip_bt_cast_prefetch_cnx.hpp"
#include "neoip_xmlrpc_listener.hpp"
#include "neoip_xmlrpc_resp.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_cast_prefetch_t::bt_cast_prefetch_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some variables
	m_xmlrpc_listener	= NULL;
	m_xmlrpc_resp		= NULL;
}

/** \brief Destructor
 */
bt_cast_prefetch_t::~bt_cast_prefetch_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// close all pending bt_cast_prefetch_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
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
bt_err_t	bt_cast_prefetch_t::start(http_listener_t *http_listener)	throw()
{
	xmlrpc_err_t	xmlrpc_err;
	// sanity check - http_listener MUST NOT be NULL
	DBG_ASSERT( http_listener );

	// build the http_uri_t on which to listen
	http_uri_t	xmlrpc_uri = "http://0.0.0.0/neoip_uri_prefetch_xmlrpc.cgi";
	http_uri_t	jsrest_uri = "http://0.0.0.0/neoip_uri_prefetch_jsrest.js";
	
	// start the xmlrpc_listener_t
	m_xmlrpc_listener= nipmem_new xmlrpc_listener_t();
	xmlrpc_err	= m_xmlrpc_listener->start(http_listener, xmlrpc_uri, jsrest_uri);
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);

	// start the xmlrpc_resp_t
	m_xmlrpc_resp	= nipmem_new xmlrpc_resp_t();
	xmlrpc_err	= m_xmlrpc_resp->start(m_xmlrpc_listener, this, NULL);
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);
	
	// add the method_name in the xmlrpc_resp_t
	m_xmlrpc_resp->insert_method("create_prefetch");
	m_xmlrpc_resp->insert_method("delete_prefetch");
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on a bt_cast_prefetch_cnx_t which match this http_uri_t
 */
bt_cast_prefetch_cnx_t * bt_cast_prefetch_t::cnx_by_http_uri(const http_uri_t &http_uri)
										const throw()
{
	std::list<bt_cast_prefetch_cnx_t *>::const_iterator	iter;
	// go thru the whole cnx_db
	for(iter = cnx_db.begin(); iter != cnx_db.end(); iter++){
		bt_cast_prefetch_cnx_t *	cnx	= *iter;
		// if this bt_cast_prefetch_cnx_t matches this http_uri, return its pointer
		if( cnx->http_uri() == http_uri )	return cnx;
	}
	// if this point is reached, no matches have been found
	return NULL;
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
bool	bt_cast_prefetch_t::neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &xmlrpc_err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw()
{
	// call the proper handler depending on the method_name
	if( method_name == "create_prefetch" ){
		xmlrpc_err_out	= xmlrpc_call_create_prefetch(xmlrpc_parse, xmlrpc_build);
	}else if( method_name == "delete_prefetch" ){
		xmlrpc_err_out	= xmlrpc_call_delete_prefetch(xmlrpc_parse, xmlrpc_build);
	}else	DBG_ASSERT( 0 );
	// return tokeep	
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xmlrpc_resp_t call handler
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief handle the method create_prefetch
 */
xmlrpc_err_t	bt_cast_prefetch_t::xmlrpc_call_create_prefetch(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw()
{
	xmlrpc_err_t	xmlrpc_err	= xmlrpc_err_t::OK;
	http_uri_t	http_uri;
	// log to debug
	KLOG_ERR("enter");
	// use xmlrpc_parse_t to get the arguments of the xmlrpc call
	NEOIP_XMLRPC_RESP_PARSE_BEGIN	(xmlrpc_parse);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, http_uri);
	NEOIP_XMLRPC_RESP_PARSE_END	(xmlrpc_parse, xmlrpc_err);
	// if there is a error in the xmlrpc_parse_t, return now
	if( xmlrpc_err.failed() )	return xmlrpc_err;

	// if the http_uri is null, return an error
	if( http_uri.is_null() )	
		return xmlrpc_err_t(xmlrpc_err_t::ERROR, "unable to parse the http uri");

	// TODO what if the http_uri is already prefetched ?!?!?!
	// - not sure... could i have 2 prefetch for a single uri ?
	// - clearly the apps will handle it
	// - leave it as such for now
	// - if a clear decision is made, document it here

	// create the prefetch_cnx for this http_uri_t
	bt_cast_prefetch_cnx_t*	prefetch_cnx;
	bt_err_t		bt_err;
	prefetch_cnx	= nipmem_new bt_cast_prefetch_cnx_t();
	bt_err		= prefetch_cnx->start(this, http_uri);
	if( bt_err.failed() ){
		nipmem_zdelete	prefetch_cnx;
		return xmlrpc_err_t(xmlrpc_err_t::ERROR, bt_err.to_string());
	}
	
	// build the xmlrpc response
	NEOIP_XMLRPC_RESP_BUILD(xmlrpc_build, uint32_t(0) );
	
	// return noerror
	return	xmlrpc_err_t::OK;
}

/** \brief handle the method delete_prefetch
 */
xmlrpc_err_t	bt_cast_prefetch_t::xmlrpc_call_delete_prefetch(xmlrpc_parse_t &xmlrpc_parse
					, xmlrpc_build_t &xmlrpc_build)		throw()
{
	xmlrpc_err_t	xmlrpc_err	= xmlrpc_err_t::OK;
	http_uri_t	http_uri;
	// log to debug
	KLOG_ERR("enter");
	// use xmlrpc_parse_t to get the arguments of the xmlrpc call
	NEOIP_XMLRPC_RESP_PARSE_BEGIN	(xmlrpc_parse);
	NEOIP_XMLRPC_RESP_PARSE_ARG	(xmlrpc_parse, http_uri);
	NEOIP_XMLRPC_RESP_PARSE_END	(xmlrpc_parse, xmlrpc_err);
	// if there is a error in the xmlrpc_parse_t, return now
	if( xmlrpc_err.failed() )	return xmlrpc_err;

	// if the http_uri is null, return an error
	if( http_uri.is_null() )	
		return xmlrpc_err_t(xmlrpc_err_t::ERROR, "unable to parse the http uri");

	// Try to find a bt_cat_prefetch_cnx_t for this http_uri_t
	bt_cast_prefetch_cnx_t*	prefetch_cnx;
	prefetch_cnx	= cnx_by_http_uri(http_uri);

	// if none exists, return an error
	if( !prefetch_cnx )
		return xmlrpc_err_t(xmlrpc_err_t::ERROR, "no prefetch for this http_uri");
	
	// else delete it
	nipmem_zdelete	prefetch_cnx;

	// build the xmlrpc response
	NEOIP_XMLRPC_RESP_BUILD(xmlrpc_build, uint32_t(0) );
	
	// return noerror
	return	xmlrpc_err_t::OK;
}


NEOIP_NAMESPACE_END;





/*! \file
    \brief Definition of the \ref udp_layer_http_t class

\par Brief Description
udp_layer_http_t handles the http display for the udp_layer_t

\par TODO 
- currently it is half backed and does display anything usefull
- it has be massively cut/paste when the udp_layer_t has been coded
  to get a slotpoll_t for udp_full_t (quite criptic)

\par Possible Improvement
- to port udp_layer_http_t as a private class of udp_layer_t

*/

/* system include */
/* local include */
#include "neoip_udp_layer_http.hpp"
#include "neoip_udp_layer.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_httpd.hpp"
#include "neoip_httpd_request.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
udp_layer_http_t::udp_layer_http_t(udp_layer_t *udp_layer)	throw()
{
	// copy the parameter
	layer	= udp_layer;
	// add the basic handler
	lib_httpd_t *	lib_http	= lib_session_get()->get_httpd();
	lib_http->handler_add(lib_http->get_rootpath() + "/udp_layer", this, NULL);	
}

/** \brief Destructor
 */
udp_layer_http_t::~udp_layer_http_t()					throw()
{
	// delete the basic handler
	lib_httpd_t *	lib_http	= lib_session_get()->get_httpd();
	lib_http->handler_del(lib_http->get_rootpath() + "/udp_layer", this, NULL);	
}


/** \brief return the root path for http
 */
std::string udp_layer_http_t::http_rootpath() const throw()
{
	return "/neoip_lib/udp_layer";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 */
httpd_err_t udp_layer_http_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request ) throw()
{
	std::string	field		= request.get_variable("field");
	std::string	operation	= request.get_variable("op");
	// log to debug
	KLOG_ERR("field=" << field << " operation=" << operation);
	// if no field is present, display the homepage
	if( field.empty() )			return homepage_http_cb(request);


// handle if field = "resp"
	if( field == "resp" ){
		if( operation == "disp_list" )	return resp_list_http_cb(request);
		return httpd_err_t::NOT_FOUND;
	}
// handle if field = "itor"
	if( field == "itor" ){
		if( operation == "disp_list" )	return itor_list_http_cb(request);
		return httpd_err_t::NOT_FOUND;
	}
// handle if field = "full"
	if( field == "full" ){
		if( operation == "disp_list" )	return full_list_http_cb(request);
		return httpd_err_t::NOT_FOUND;
	}
		
	return httpd_err_t::NOT_FOUND;
}

/** \brief display the page header
 */
std::string udp_layer_http_t::page_header()	throw()
{
	std::ostringstream	oss;
	oss << h.b("UDP Socket:");
	oss << " "	<< resp_link("Responder: " + resp_summary(), "disp_list");
	oss << " / "	<< itor_link("Initiator: " + itor_summary(), "disp_list");
	oss << " / "	<< full_link("Full: " + full_summary(), "disp_list");
	return oss.str();	
}


/** \brief callback notified when a request is received by this neoip_htt_handler
 */
httpd_err_t udp_layer_http_t::homepage_http_cb(httpd_request_t &request)	throw()
{
	std::ostringstream	&oss = request.get_reply();
	oss << page_header();
	oss << h.pagetitle("UDP Socket Layer");
	oss << h.br();
	oss << "This section describes all the currently used udp_resp/itor/full_t objects.";
	oss << " See the page header.";
	return httpd_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                               resp
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief return a summary of the resp
 */
std::string udp_layer_http_t::resp_summary()				throw()
{
	std::ostringstream	oss;
	oss << layer->udp_resp_db.size();
	return oss.str();	
}

/** \brief return a link toward the resp
 */
std::string udp_layer_http_t::resp_link(const std::string &label
						, const std::string &operation)	throw()
{
	return h.s_link(http_rootpath() + "?field=resp&op=" + operation ) + label + h.e_link();
}

/** \brief display a list of all resp
 */
httpd_err_t udp_layer_http_t::resp_list_http_cb(httpd_request_t &request)	throw()
{
	std::list<udp_resp_t *>::iterator	iter;
	std::ostringstream &			oss = request.get_reply();
	
	oss << page_header();
	oss << h.pagetitle("UDP Socket Responder");
	oss << h.br();
	return httpd_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                               itor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief return a summary of the itor
 */
std::string udp_layer_http_t::itor_summary()				throw()
{
	std::ostringstream	oss;
	oss << layer->udp_itor_db.size();
	return oss.str();	
}

/** \brief return a link toward the itor
 */
std::string udp_layer_http_t::itor_link(const std::string &label
						, const std::string &operation)	throw()
{
	return h.s_link(http_rootpath() + "?field=itor&op=" + operation ) + label + h.e_link();
}

/** \brief display a list of all itor
 */
httpd_err_t udp_layer_http_t::itor_list_http_cb(httpd_request_t &request)	throw()
{
	std::list<udp_itor_t*>::iterator	iter;
	std::ostringstream &			oss = request.get_reply();
	
	oss << page_header();
	oss << h.pagetitle("UDP Socket Initiator");
	oss << h.br();
	return httpd_err_t::OK;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                               full
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief return a summary of the full
 */
std::string udp_layer_http_t::full_summary()				throw()
{
	std::ostringstream	oss;
	oss << layer->udp_full_db.size();
	return oss.str();	
}

/** \brief return a link toward the full
 */
std::string udp_layer_http_t::full_link(const std::string &label
						, const std::string &operation)	throw()
{
	return h.s_link(http_rootpath() + "?field=full&op=" + operation ) + label + h.e_link();
}

/** \brief display a list of all full
 */
httpd_err_t udp_layer_http_t::full_list_http_cb(httpd_request_t &request)	throw()
{
	std::list<udp_full_t*>::iterator	iter;
	std::ostringstream &			oss = request.get_reply();
	
	oss << page_header();
	oss << h.pagetitle("UDP Socket Full");
	oss << h.br();
	
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END






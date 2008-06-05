/*! \file
    \brief Definition of the \ref bud_main_t::http_search_t class

\par Brief Description
This modules implements the search in buddy via http

*/

/* system include */
/* local include */
#include "neoip_bud_http_search.hpp"
#include "neoip_kad.hpp"
#include "neoip_httpd.hpp"
#include "neoip_html_builder.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_lib_session.hpp"	// TODO to remove - there are no reason to work only on the
#include "neoip_lib_httpd.hpp"		// session httpd and not any httpd


NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bud_main_t::http_search_t::http_search_t(bud_main_t *bud_main)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// copy some parameter
	this->bud_main	= bud_main;
	
	// zero some field
	kad_query	= NULL;
	// link itself to bud_main_t
	bud_main->http_search_link(this);
}

/** \brief Destructor
 */
bud_main_t::http_search_t::~http_search_t()			throw()
{
	// log to debug
	KLOG_ERR("enter");
	// delete the kad_query_t if needed
	nipmem_zdelete	kad_query;
	// unlink itself from bud_main_t
	bud_main->http_search_unlink(this);
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start The operation
 * 
 * - this may delete the http_search_t
 */
httpd_err_t bud_main_t::http_search_t::start(httpd_request_t &request)	throw()
{
	kad_peer_t *	kad_peer	= bud_main->kad_peer;
	kad_err_t	kad_err;
	// log to debug
	KLOG_ERR("request method=" << request.get_method() << " path=" << request.get_path() );

	std::string	keyword_str	= request.get_variable("search_keyword");

	if( keyword_str.empty() )	return httpd_err_t::NOT_FOUND;
/* TODO
 * - get the search criteria from the httpd_request_t
 * - compute the keyid from it
 * - launch the kad_listener getall
 * - reply a httpd_err_t::DELAYED_REPLY
 * 
 * - when the command reply
 *   - build the html page from the reply
 */
	kad_keyid_t	kad_keyid	= keyword_str.c_str();

	// start the getall command
	kad_query	= nipmem_new kad_query_t();
	kad_err		= kad_query->start(kad_peer, kad_keyid, 0, delay_t::from_sec(30), this, NULL);
	DBG_ASSERT( kad_err.succeed() );
	

	httpd_request	= request;
	return httpd_err_t::DELAYED_REPLY;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_query_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_query_t has an event to notify
 */
bool	bud_main_t::http_search_t::neoip_kad_query_cb(void *cb_userptr, kad_query_t &kad_query
						, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event MUST be is_query_ok()
	DBG_ASSERT( kad_event.is_query_ok() );

	httpd_err_t	httpd_err	= build_http_reply(kad_event);
	// notify the reply
	// TODO bug here - why should you reply to the session httpd and not another one ?
	// - possible solution to put a httpd_t backpoint in the request ?
	lib_session_get()->get_httpd()->notify_delayed_reply(httpd_request, httpd_err);
	// autodelete\
	nipmem_delete this;
	// return 'dontkeep'
	return false;
}

/** \brief build the http reply from the kad_event_t
 */
httpd_err_t	bud_main_t::http_search_t::build_http_reply(const kad_event_t &kad_event)	throw()
{
	std::ostringstream	&oss = httpd_request.get_reply();
	html_builder_t		h;

	// sanity check - the kad_event MUST match the command
	DBG_ASSERT( kad_event.is_getall_ok() );	
	
	oss << h.pagetitle("Buddy Search");
	oss << h.br();
	
	// if the kad command failed, display the error on the page
	if( !kad_event.is_recdups() ){
		oss << "Failed to find";
		return httpd_err_t::OK;
	}
	
	// get the kad_recdups_t data
	bool			has_more_record;
	const kad_recdups_t &	kad_recdups = kad_event.get_recdups(&has_more_record);
	
	oss << h.s_table();
	
	for( size_t i = 0; i < kad_recdups.size(); i++ ){
		std::string	url_str = string_t::from_datum(kad_recdups[i].get_value());
		oss << h.s_tr() << h.s_td();
		oss << h.s_link(url_str);
		oss << "record " << url_str;
		oss << h.e_link();
		oss << h.e_td() << h.e_tr();
	}
	
	oss << h.e_table();
	return httpd_err_t::OK;
}
NEOIP_NAMESPACE_END






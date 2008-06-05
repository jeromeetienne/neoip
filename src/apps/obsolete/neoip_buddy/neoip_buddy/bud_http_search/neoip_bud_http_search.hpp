/*! \file
    \brief Header of the \ref bud_main_t class
    
*/


#ifndef __NEOIP_BUD_HTTP_SEARCH_HPP__ 
#define __NEOIP_BUD_HTTP_SEARCH_HPP__ 
/* system include */
/* local include */
#include "neoip_bud_main.hpp"
#include "neoip_kad_query_cb.hpp"
#include "neoip_httpd_request.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	kad_listener_t;

/** \brief Define the context for the session opened via the xmlrpc
 */
class bud_main_t::http_search_t : NEOIP_COPY_CTOR_DENY, private kad_query_cb_t {
private:
	bud_main_t *	bud_main;	//!< back pointer on the bud_main_t
	httpd_request_t httpd_request;	//!< contain the httpd_request_t which triggered 
					//!< this command.
	
	/*************** kad_query_t	***************************************/
	kad_query_t *	kad_query;
	bool		neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw();

	httpd_err_t	build_http_reply(const kad_event_t &kad_event)		throw();	
public:
	/*************** ctor/dtor	***************************************/
	http_search_t(bud_main_t *bud_main)	throw();
	~http_search_t()			throw();
	
	/*************** setup function 	*******************************/
	httpd_err_t	start(httpd_request_t &request)	throw();	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BUD_HTTP_SEARCH_HPP__  */




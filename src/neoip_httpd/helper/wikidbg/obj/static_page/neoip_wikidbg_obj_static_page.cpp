/*! \file
    \brief Definition of the \ref wikidbg_obj_static_page_t class

*/

/* system include */
/* local include */
#include "neoip_wikidbg_obj_static_page.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_httpd.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
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
wikidbg_obj_static_page_t::wikidbg_obj_static_page_t(const std::string &url_path, void *object_ptr)	throw()
{
	lib_httpd_t *lib_http	= lib_session_get()->get_httpd();
	// copy the parameter
	this->url_path		= lib_http->get_rootpath() + url_path;
	this->object_ptr	= object_ptr;
	// add the basic handler
	lib_http->handler_add(this->url_path, this, NULL);
}

/** \brief Destructor
 */
wikidbg_obj_static_page_t::~wikidbg_obj_static_page_t()					throw()
{
	lib_httpd_t *lib_http	= lib_session_get()->get_httpd();
	// remote the handler from httpd
	lib_http->handler_del(url_path, this, NULL);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback notified when a httpd request is received by this neoip_httpd_handler_cb
 */
httpd_err_t wikidbg_obj_static_page_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request) throw()
{
	// redirect on the wikidbg page for wikidbg_obj
	return httpd_err_t(httpd_err_t::MOVED_TEMP, wikidbg_url("page", object_ptr));
}


NEOIP_NAMESPACE_END






/*! \file
    \brief Definition of shortcut for using the wikidbg_http_t of the lib session

\par Note about constness
- all the functions receive const void * pointer and immediatly after remove
  the const.
  - it is due to the fact that the functions MUST accept const pointer
    but pass non const pointer to the wikidbg callback.
  - so somewhere in the process the const must be removed, it has been chosen
    to remove it here

*/

/* system include */
/* local include */
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_lib_httpd.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_httpd.hpp"
#include "neoip_wikidbg_global_db.hpp"
#include "neoip_wikidbg_http.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Return the url associated with this keyword/object_ptr
 */
std::string	wikidbg_url(const std::string &keyword, const void *c_obj_ptr)	throw()
{
	void *			object_ptr	= const_cast<void *>(c_obj_ptr);	
	const wikidbg_http_t * wikidbg_http	= lib_session_get()->get_httpd()->get_wikidbg();
	// build the url to point on this keyword/object_ptr
	return wikidbg_http->build_url(keyword, object_ptr);
}

/** \brief return the html associted with this keyword/object_ptr
 */
std::string	wikidbg_html(const std::string &keyword, const void *c_obj_ptr)	throw()
{
	void *			object_ptr	= const_cast<void *>(c_obj_ptr);	
	return wikidbg_global_db_t::get_keyword_html(keyword, object_ptr);	
}

/** \brief return the html for hidden variable to put in a form if the destination is a wikidbg_url
 */
std::string	wikidbg_form_hidden_param(const std::string &keyword, const void *c_obj_ptr)	throw()
{
	void *			object_ptr	= const_cast<void *>(c_obj_ptr);	
	const wikidbg_http_t * wikidbg_http	= lib_session_get()->get_httpd()->get_wikidbg();
	// build the form hidden variable to point on this keyword/object_ptr
	return wikidbg_http->build_form_hidden_param(keyword, object_ptr);
}

/** \brief Return the root path for the wikidbg url
 */
std::string	wikidbg_url_rootpath()	throw()
{
	const wikidbg_http_t * wikidbg_http	= lib_session_get()->get_httpd()->get_wikidbg();
	// return the url path
	return wikidbg_http->get_url_path();
}

/** \brief return the html associted with this keyword/object_ptr
 */
std::string	wikidbg_html_callback_typename(const void *c_obj_ptr
						, const std::string &typename_str)	throw()
{
	void *			object_ptr	= const_cast<void *>(c_obj_ptr);	
	std::ostringstream	oss;
	html_builder_t		h;
	// test if there is a wikidbg 'page' for this object_ptr	
	bool	page_exist	= wikidbg_global_db_t::contain_keyword_page("page", object_ptr);
	
	// Start the link if there is a wikidbg page for it
	if( page_exist ){
		// compute the title attribute if any is available
		std::string		attribute;	
		if( wikidbg_global_db_t::contain_keyword_html("page_title_attr", object_ptr) )
			attribute	= wikidbg_html("page_title_attr", object_ptr);
		// Start the link
		oss << h.s_link(wikidbg_url("page", object_ptr), attribute);
	}

	// put the typename of the object_ptr	
	oss << typename_str;
	// if there is a 'oneword' html keyword in the wikidbg, put it else put the address
	oss << "(";
	if( wikidbg_global_db_t::contain_keyword_html("oneword", object_ptr ) ){
		oss << wikidbg_html("oneword", object_ptr);
	}else{
		oss << object_ptr;
	}
	oss << ")";

	// end the link if needed
	if( page_exist )	oss << h.e_link();
	
	// return the just built string
	return oss.str();
}
NEOIP_NAMESPACE_END






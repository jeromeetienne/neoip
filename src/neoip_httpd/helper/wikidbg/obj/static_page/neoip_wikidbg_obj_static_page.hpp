/*! \file
    \brief Header of the \ref wikidbg_obj_static_page_t class
    
*/


#ifndef __NEOIP_WIKIDBG_OBJ_STATIC_PATH_HPP__ 
#define __NEOIP_WIKIDBG_OBJ_STATIC_PATH_HPP__ 
/* system include */
/* local include */
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement the wikidbg_obj_t debug display on http
 */
class wikidbg_obj_static_page_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t {
private:
	std::string	url_path;
	void *		object_ptr;

	/*************** httpd callback	***************************************/
	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();
public:
	/*************** ctor/dtor	***************************************/
	wikidbg_obj_static_page_t(const std::string &url_path, void *object_ptr)	throw();
	~wikidbg_obj_static_page_t()							throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_WIKIDBG_OBJ_STATIC_PATH_HPP__  */




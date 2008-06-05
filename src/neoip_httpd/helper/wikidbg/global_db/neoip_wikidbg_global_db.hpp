/*! \file
    \brief Declaration of the wikidbg_obj_t

*/


#ifndef __NEOIP_WIKIDBG_GLOBAL_DB_HPP__ 
#define __NEOIP_WIKIDBG_GLOBAL_DB_HPP__ 
/* system include */
#include <map>
#include <list>
/* local include */
#include "neoip_wikidbg_init_cb.hpp"
#include "neoip_httpd_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class httpd_request_t;

/** \brief list of static function to handle the global wikidbg_obj_t database
 */
class wikidbg_global_db_t {
public:
	/*************** database function	*******************************/
	static void	insert(void *object_ptr, wikidbg_init_cb_t callback
				, const std::list<void *> &alias_ptr_db = std::list<void *>())	throw();
	static void 	remove(void *object_ptr	
				, const std::list<void *> &alias_ptr_db = std::list<void *>())	throw();
	static wikidbg_init_cb_t	find(void *object_ptr)					throw();

	/*************** handler function	*******************************/
	static std::string get_keyword_html(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t get_keyword_page(const std::string &keyword, void *object_ptr
							, httpd_request_t &httpd_request)	throw();
	
	/*************** contain function	*******************************/
	static bool	contain_keyword_html(const std::string &keyword, void *object_ptr)	throw();
	static bool	contain_keyword_page(const std::string &keyword, void *object_ptr)	throw();		
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_WIKIDBG_GLOBAL_DB_HPP__  */




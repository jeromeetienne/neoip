/*! \file
    \brief Declaration of the wikidbg_keyword_db_t

*/


#ifndef __NEOIP_WIKIDBG_KEYWORD_DB_HPP__ 
#define __NEOIP_WIKIDBG_KEYWORD_DB_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_httpd_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class httpd_request_t;

/** \brief The callback type to handle a given keyword attached to a object_ptr to produce plain html
 */
typedef std::string (*wikidbg_keyword_html_cb_t)(const std::string &keyword, void *object_ptr);

/** \brief The callback type to handle a given keyword attached to a object_ptr to produce a full http req
 */
typedef httpd_err_t (*wikidbg_keyword_page_cb_t)(const std::string &keyword, void *object_ptr
								, httpd_request_t &httpd_request);

/** \brief \ref wikidbg_keyword_db_t stores the keyword and their specific handler callback
 */
class wikidbg_keyword_db_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::map<std::string, wikidbg_keyword_html_cb_t>	html_db;
	std::map<std::string, wikidbg_keyword_page_cb_t>	page_db;
public:
	/*************** Function to handle the html_cb	***********************/
	wikidbg_keyword_db_t &		insert_html(const std::string &keyword
						, wikidbg_keyword_html_cb_t callback)	throw();
	wikidbg_keyword_db_t &		remove_html(const std::string &keyword)			throw();
	wikidbg_keyword_html_cb_t	find_html(const std::string &keyword)		const throw();

	/*************** Function to handle the page_cb	***********************/
	wikidbg_keyword_db_t &		insert_page(const std::string &keyword
						, wikidbg_keyword_page_cb_t callback)	throw();
	wikidbg_keyword_db_t &		remove_page(const std::string &keyword)			throw();
	wikidbg_keyword_page_cb_t	find_page(const std::string &keyword)		const throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_WIKIDBG_KEYWORD_DB_HPP__  */




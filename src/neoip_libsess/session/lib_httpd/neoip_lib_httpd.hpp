/*! \file
    \brief Header of the \ref lib_httpd_t class
    
*/


#ifndef __NEOIP_LIB_HTTPD_HPP__ 
#define __NEOIP_LIB_HTTPD_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <map>
/* local include */
#include "neoip_httpd_handler.hpp"
#include "neoip_httpd_err.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	lib_session_t;
class	httpd_t;
class	wikidbg_http_t;
class	nipmem_tracker_http_t;
class	file_path_t;
class	file_err_t;

/** \brief define a handler for a specific http path
 */
class lib_httpd_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t  {
private:
	std::string	rootpath;	//!< the http root patch
	lib_session_t *	lib_session;	//!< back pointer on the lib_session
	
	nipmem_tracker_http_t *	nipmem_tracker_http;	//!< http stuff to display content of nipmem_tracker

	/*************** httpd_t callback	*******************************/
	httpd_t *	httpd;
	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();
	
	/*************** wikidbg stuff	***************************************/
	wikidbg_http_t*	wikidbg_http;

	/*************** internal function	*******************************/
	httpd_err_t	handler_root_cb(httpd_request_t &request)		throw();
	httpd_err_t	handler_apps_info_cb(httpd_request_t &request)		throw();
	httpd_err_t	handler_apps_info_jsrest_cb(httpd_request_t &request)	throw();
	httpd_err_t	handler_list_cb(httpd_request_t &request)		throw();
	httpd_err_t	handler_static_file_cb(httpd_request_t &request)	throw();
	ipport_addr_t	listen_addr_from_conf()					throw();	
public:
	/*************** ctor/dtor	***************************************/
	lib_httpd_t(lib_session_t *lib_session)		throw();
	~lib_httpd_t()					throw();
	
	/*************** Setup function	***************************************/
	bool			start()			throw();
	
	/*************** Query function	***************************************/
	const std::string &	get_rootpath() 		const throw()	{ return rootpath;	}
	const wikidbg_http_t *	get_wikidbg()		const throw()	{ return wikidbg_http;	}
	ipport_addr_t		get_listen_addr()	const throw();
	

	/*************** Handler management	*******************************/
	void	handler_add(std::string path, httpd_handler_cb_t *callback, void *userptr
				, httpd_handler_flag_t handler_flag = httpd_handler_t::FLAG_DFL)	throw();
	void	handler_del(std::string path, httpd_handler_cb_t *callback, void *userptr
				, httpd_handler_flag_t handler_flag = httpd_handler_t::FLAG_DFL)	throw();	
	void	notify_delayed_reply(const httpd_request_t &request, const httpd_err_t &httpd_err)	throw();

};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_HTTPD_HPP__  */




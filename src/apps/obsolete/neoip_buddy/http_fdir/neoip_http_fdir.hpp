/*! \file
    \brief Header of the \ref http_fdir_t class
    
*/


#ifndef __NEOIP_HTTP_FDIR_HPP__ 
#define __NEOIP_HTTP_FDIR_HPP__ 
/* system include */
#include <list>
#include <map>
/* local include */
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_html_builder.hpp"
#include "neoip_file_path.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// just to debug
#include "neoip_file_dir.hpp"

NEOIP_NAMESPACE_BEGIN;


// list of forward declaration
class mimediag_t;
class thumb_factory_t;
class httpd_auth_t;

/** \brief define a handler for a specific http path
 */
class http_fdir_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t {
private:
	html_builder_t	h;	//!< a helper to build html easily in this object
	class		uri_var_t;

	file_path_t	url_rootpath;
	file_path_t	file_rootpath;
	file_path_t	thumb_prefix;
	std::string	root_alias;
	
	mimediag_t *		mimediag;
	thumb_factory_t *	thumb_factory;
	
	/*************** authentication	***************************************/
	httpd_auth_t *				httpd_auth;
	std::map<std::string, std::string>	user_db;
	struct		fdir_auth_get_pass_ftor_t;

	/*************** header function	*******************************/
	std::string	path_selector_html(httpd_request_t &request, const file_path_t &file_path)	throw();


	// to handle irl variable	
	std::string	dir_order_html(httpd_request_t &request, const file_path_t &file_path)	throw();
	std::string	dir_view_html(httpd_request_t &request, const file_path_t &file_path)	throw();
	std::string	show_hidden_html(httpd_request_t &request, const file_path_t &file_path)throw();

	// to handle the url 
	std::string	get_url_path(httpd_request_t &request,const file_path_t &file_relpath
					, const std::string &anchor_str = "")	const throw();
	/*************** httpd handler callback	*******************************/
	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();
	
	/*************** handler for directory	*******************************/
	httpd_err_t	handle_view_list(httpd_request_t &request, const file_path_t &dir_relpath)
										throw();	
	httpd_err_t	handle_view_thumbnail(httpd_request_t &request, const file_path_t &dir_relpath)
										throw();	
	httpd_err_t	handle_view_imageshow(httpd_request_t &request, const file_path_t &dir_relpath)
										throw();	
	httpd_err_t	handle_view_getplaylist(httpd_request_t &request, const file_path_t &dir_relpath)
										throw();	
										
	/*************** handler for file	*******************************/
	httpd_err_t	handle_file_direct(httpd_request_t &request, const file_path_t &file_relpath)	
										throw();

	/*************** handler for thumbnail	*******************************/
	bool		relpath_is_thumb(const file_path_t &relpath)				const throw();
	httpd_err_t	handle_thumb(httpd_request_t &request, const file_path_t &file_relpath)	throw();	

	void		put_html_head(httpd_request_t &request, const file_path_t &dir_relpath) throw();
	void		put_html_tail(httpd_request_t &request, const file_path_t &dir_relpath) throw();
	std::string	add_nonspacebreak(const std::string &str, size_t max_len)		const throw();
	bool		is_text_browser(const httpd_request_t &request)		const throw();

	httpd_err_t	download_file_fullpath(httpd_request_t &request, const file_path_t &file_path)
										throw();
public:
	/*************** ctor/dtor	***************************************/
	http_fdir_t()	throw();
	~http_fdir_t()	throw();
		
	/*************** setup function	***************************************/
	void	set_user_db(const std::map<std::string, std::string> &user_db)	throw();
	bool	start(const std::string &url_rootpath, const file_path_t &file_rootpath
						, const std::string &root_alias = "")	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_FDIR_HPP__  */




/*! \file
    \brief Header of the \ref http_fdir_t::uri_var_t

- see \ref neoip_pktcomp_profile.cpp
*/


#ifndef __NEOIP_HTTP_FDIR_URI_VAR_HPP__ 
#define __NEOIP_HTTP_FDIR_URI_VAR_HPP__ 
/* system include */
/* local include */
#include "neoip_http_fdir.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class http_fdir_t::uri_var_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::string	dir_order_val;
	std::string	dir_view_val;
	std::string	show_hidden_val;
public:
	/*************** ctor/dtor	***************************************/
	uri_var_t(const httpd_request_t &httpd_request)	throw();
	~uri_var_t()					throw();

	bool			is_null()	const throw() { return dir_order_val == "";	};

	std::string		to_url()	const throw();

	/*************** query function	***************************************/
	const std::string &	dir_view()	const throw()		{ return dir_view_val;};
	std::string &		dir_view()	throw()			{ return dir_view_val;};
	const std::string &	dir_order()	const throw()		{ return dir_order_val;};
	std::string &		dir_order()	throw()			{ return dir_order_val;};
	const std::string &	show_hidden()	const throw()		{ return show_hidden_val;};
	std::string &		show_hidden()	throw()			{ return show_hidden_val;};

};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_FDIR_URI_VAR_HPP__  */




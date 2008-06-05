/*! \file
    \brief Definition of the \ref pktcomp_profile_t

*/


/* system include */
/* local include */
#include "neoip_http_fdir_uri_var.hpp"
#include "neoip_httpd_request.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor from httpd_request_t
 */
http_fdir_t::uri_var_t::uri_var_t(const httpd_request_t &httpd_request)	throw()
{
	// read the values from the http_request variables
	dir_view_val	= httpd_request.get_variable("dir_view"		, "plainlist");
	dir_order_val	= httpd_request.get_variable("dir_order"	, "n");
	show_hidden_val	= httpd_request.get_variable("show_hidden"	, "n");
}

/** \brief Destructor
 */
http_fdir_t::uri_var_t::~uri_var_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                      convertion
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the uri_var_t into a url format
 */
std::string	http_fdir_t::uri_var_t::to_url()	const throw()
{
	std::ostringstream	oss;
	oss << "dir_view=" << dir_view_val;
	oss << "&";
	oss << "dir_order=" << dir_order_val;
	oss << "&";
	oss << "show_hidden=" << show_hidden_val;	
	// return the resulting string
	return oss.str();
}

NEOIP_NAMESPACE_END


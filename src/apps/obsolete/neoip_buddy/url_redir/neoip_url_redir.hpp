/*! \file
    \brief Header of the url_redir_t class
    
*/


#ifndef __NEOIP_URL_REDIR_HPP__ 
#define __NEOIP_URL_REDIR_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_dnsgrab_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

#include "neoip_zerotimer.hpp"	// TODO to remove when the dnsgrab support sync operation
				// - i think it already does
#include "neoip_dnsgrab.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Class to do url redirection
 */
class url_redir_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t, private dnsgrab_cb_t {
private:
	httpd_t	*	httpd_srv;	//!< the http server for this redirector
	dnsgrab_t *	dnsgrab;

	std::map<std::string, std::string>	hosturl_db;	//!< store the url indexed by
								//!< by hostname

	void		add_hosturl(const std::string &host_str, const std::string &url_str )	throw();
	std::string	get_hosturl(const std::string &host_str)				const throw();

	/*************** httpd handler callback	*******************************/
	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();
	bool		neoip_dnsgrab_cb(void *cb_userptr, dnsgrab_request_t &request)		throw();
	
public:
	/*************** ctor/dtor	***************************************/
	url_redir_t()	throw();
	~url_redir_t()	throw();
	
	bool	start()	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_URL_REDIR_HPP__  */




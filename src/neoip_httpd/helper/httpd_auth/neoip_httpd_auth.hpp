/*! \file
    \brief Header of the neoip_httpd_auth class
    
*/


#ifndef __NEOIP_HTTPD_AUTH_HPP__ 
#define __NEOIP_HTTPD_AUTH_HPP__ 
/* system include */
/* local include */
#include "neoip_httpd_err.hpp"
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class httpd_request_t;

/** \brief Class to authenticate a given realm
 * 
 * - a realm here is a http authentication realm as in rfc2617.3.2.1
 */
class httpd_auth_t : NEOIP_COPY_CTOR_DENY {
public:
	/** \brief The httpd_auth_t functor which return false if the username doesnt exists
	 *         and if it does, return true and copy the password into the parameter
	 */
	class get_pass_ftor_t {
	public:	virtual bool operator()(const std::string &username, const httpd_auth_t &cb_httpd_auth
					, const httpd_request_t &cb_httpd_request
					, std::string &password) throw() = 0;
		virtual ~get_pass_ftor_t() {};
	};

private:
	std::string	auth_realm;	//!< the authentication realm 
	std::string	auth_scheme;	//!< the authentication scheme to use "basic" or "digest"
	datum_t		digest_secret;	//!< the digest secret in case of a "digest" auth_scheme

	httpd_err_t	is_allowed_digest(httpd_request_t &httpd_request
				, httpd_auth_t::get_pass_ftor_t &get_pass_ftor)	const throw();
	httpd_err_t	is_allowed_basic(httpd_request_t &httpd_request
				, httpd_auth_t::get_pass_ftor_t &get_pass_ftor)	const throw();
	std::string	cpu_digest_nonce(httpd_request_t &httpd_request)		const throw();

public:
	/*************** ctor/dtor	***************************************/
	httpd_auth_t(const std::string &auth_realm, const std::string &auth_scheme)	throw();
	~httpd_auth_t()									throw();
	
	/*************** query function	***************************************/
	httpd_err_t	is_allowed(httpd_request_t &httpd_request
				, httpd_auth_t::get_pass_ftor_t &get_pass_ftor)	const throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTPD_AUTH_HPP__  */




/*! \file
    \brief Definition of the \ref httpd_auth_t class

\par Brief Description
This module allows to verify the authentication via rfc 2517 "HTTP Authentication: Basic
and Digest Access Authentication"

*/

/* system include */
/* local include */
#include "neoip_httpd_auth.hpp"
#include "neoip_httpd_request.hpp"
#include "neoip_base64.hpp"
#include "neoip_skey_auth.hpp"
#include "neoip_random_pool.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
httpd_auth_t::httpd_auth_t(const std::string &auth_realm, const std::string &auth_scheme)	throw()
{
	// copy the parameter
	this->auth_realm	= auth_realm;
	this->auth_scheme	= auth_scheme;
	// sanity check - auth_scheme MUST be equal to either "basic" or "digest"
	DBG_ASSERT( auth_scheme == "basic" || auth_scheme == "digest" );
	
	// if the auth_scheme is digest, get a random secret
	if( auth_scheme == "digest" ){
		digest_secret	= datum_t(16);
		random_pool_t::read_normal(digest_secret.get_data(), digest_secret.get_len());		
	}
}

/** \brief Destructor
 */
httpd_auth_t::~httpd_auth_t()	throw()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//              to test if a httpd_request_t is authenticated
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

httpd_err_t	httpd_auth_t::is_allowed(httpd_request_t &request
				, httpd_auth_t::get_pass_ftor_t &get_pass_ftor)	const throw()
{
	if( auth_scheme == "digest" )	return is_allowed_digest(request, get_pass_ftor);
	else				return is_allowed_basic(request, get_pass_ftor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//        to test if a httpd_request_t is authenticated via DIGEST scheme
// TODO: it is unclear that this works well, the authentication is made but
//       browser keep asking for the password
//       - maybe it is my way to compute the nonce...
//       - this is indead the issue
//       - how to compute the nonce ?
//       - see and fix cpu_digest_nonce()
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief perform the authentication using the "digest" authentication scheme
 */
httpd_err_t	httpd_auth_t::is_allowed_digest(httpd_request_t &request
				, httpd_auth_t::get_pass_ftor_t &get_pass_ftor)	const throw()
{
	// log to debug
	KLOG_DBG("header=" << request.get_header_property() );	
	// compute the nonce of this request
	std::string	nonce	= cpu_digest_nonce(request);
	// build the challenge to reply if this request is not authenticated
	std::string	challenge_str = "Digest realm=\"" + auth_realm + "\"";
	challenge_str	+= ", qop=\"auth\"";		// use the "auth" quality of protection
	challenge_str	+= ", nonce=\"" + nonce + "\"";
	// set the error to return if the request is not allowed
	httpd_err_t	unauth_err = httpd_err_t(httpd_err_t::UNAUTHORIZED, challenge_str);
	
	// get the authorization string in the header
	std::string	auth_str	= request.get_header("Authorization");	
	// if there is no authorization string in the header, return unauthorized
	if( auth_str.empty() )		return unauth_err;
	// parse the auth_str	
	std::vector<std::string>	words	= string_t::split( auth_str, " ", 2);
	// if the first word is not "Basic", return unauthorized
	if( string_t::casecmp(words[0], "Digest") )	return unauth_err;
	if( words.size() != 2 )				return unauth_err;

	KLOG_DBG("attr=" << words[1]);

	
	std::string	username, password;
	std::string	req_uri, req_nonce, req_nc_value;
	std::string	req_qop, req_cnonce;
	std::string	req_realm, req_response;
	std::string	tmp;


	// parse the auth_str
	std::vector<std::string>	attr	= string_t::split( words[1], ",");
	for( size_t i = 0; i < attr.size(); i++ ){
		// strop the string
		attr[i]	= string_t::strip(attr[i]);
		// split the string into varname=varvalue
		std::vector<std::string>	var	= string_t::split(attr[i], "=", 2);
		// if this var has less than 2 elements, skip it
		if( var.size() != 2 )	continue;
		KLOG_DBG("attr[" << i << "]=" << attr[i] << " var=" << var[0] << " value=" << var[1]);
		// handle each case of varname
		if( var[0] == "username" ){
			username	= string_t::unquote(var[1]);
		}else if( var[0] == "realm" ){
			req_realm	= string_t::unquote(var[1]);
		}else if( var[0] == "nonce" ){
			req_nonce	= string_t::unquote(var[1]);
		}else if( var[0] == "uri" ){
			req_uri		= string_t::unquote(var[1]);
		}else if( var[0] == "response" ){
			req_response	= string_t::unquote(var[1]);
		}else if( var[0] == "qop" ){
			req_qop		= var[1];
		}else if( var[0] == "nc" ){
			req_nc_value	= var[1];
		}else if( var[0] == "cnonce" ){
			req_cnonce	= string_t::unquote(var[1]);
		}
	}

	// get the user_password
	std::string	user_password;
	if( get_pass_ftor(username, *this, request, user_password) == false )
		return unauth_err;
	
	// init the hash function
	skey_auth_t	skey_auth("md5/nokey/16");

	// if realm is the request doesnt the local one, reject the request
	if( req_realm != auth_realm )	return unauth_err;
	// if req_qop doesnt match "auth", reject the request
	if( req_qop != "auth" )		return unauth_err;
	// if req_nonce doesnt match real nonce, reject the request
	if( req_nonce != nonce )	return unauth_err;

// compute A1 for "MD5" algorithm - see rfc2617.3.2.2.2
	skey_auth.init_key(NULL, 0);	
	tmp	= username + ":" + req_realm + ":" + user_password;
	skey_auth.update( tmp.c_str(), tmp.size() );
	std::string	hash_a1	= skey_auth.get_output().to_string().substr(2);
	KLOG_DBG("a1 computed over=" << tmp << " hash_a1=" << hash_a1);

// compute A2 for "auth" quality of protection - see rfc2617.3.2.2.3
	skey_auth.init_key(NULL, 0);
	tmp	= request.get_method() + ":" + req_uri;
	skey_auth.update( tmp.c_str(), tmp.size() );
	std::string	hash_a2	= skey_auth.get_output().to_string().substr(2);
	KLOG_DBG("a2 computed over=" << tmp << " hash_a2=" << hash_a2);

// compute request-digest for "auth" quality of protection - see rfc2617.3.2.2.1
	// prepend the secret to compute KD() - see rfc2617.3.2.1.algorithm
	tmp	  = hash_a1;
	tmp	 += ":" + req_nonce + ":" + req_nc_value + ":" + req_cnonce
			+ ":" + req_qop;
	tmp	 += ":" + hash_a2;
	skey_auth.init_key(NULL, 0);
	skey_auth.update( tmp.c_str(), tmp.size() );
	// get the result
	std::string	request_digest	= skey_auth.get_output().to_string().substr(2);
	// log to debug
	KLOG_DBG("request_digest computed over=" << tmp << " request digest=" << request_digest );
	
	// deny access if the recomputed request_digest is different from the request's "response" field 
	if( request_digest != req_response )	return unauth_err;

	// if this point is reached, the username/password are allowed, so return httpd_err_t::OK	
	return httpd_err_t::OK;
}

/** \brief Compute the digest nonce from the httpd_request_t - see rfc2617.3.2.1.nonce
 * 
 * - the nounce is hmac(digest_secret, uri)
 */
std::string	httpd_auth_t::cpu_digest_nonce(httpd_request_t &httpd_request)		const throw()
{
	skey_auth_t	skey_auth("md5/hmac/16");
//	std::string	uri	= httpd_request.get_path();
// TODO how to compute the nonce ?
// - the nounce can not depends on the path or it will be reask for each new path
// - maybe i misundertood something... like maybe with the stale=true
	std::string	uri	= "bonjour";
	skey_auth.init_key( digest_secret );
	skey_auth.update( uri.c_str(), uri.size() );
	return skey_auth.get_output().to_string().substr(2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//        to test if a httpd_request_t is authenticated via BASIC scheme
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief perform the authentication using the "basic" authentication scheme
 */
httpd_err_t	httpd_auth_t::is_allowed_basic(httpd_request_t &request
				, httpd_auth_t::get_pass_ftor_t &get_pass_ftor)	const throw()
{
	// set the error to return if the request is not allowed
	httpd_err_t	unauth_err = httpd_err_t(httpd_err_t::UNAUTHORIZED, "Basic realm=\"" + auth_realm + "\"");
	// get the authorization string in the header
	std::string	auth_str	= request.get_header("Authorization");	
	// if there is no authorization string in the header, return unauthorized
	if( auth_str.empty() )		return unauth_err;

	// parse the auth_str	
	std::vector<std::string>	words	= string_t::split( auth_str, " ", 2);
	// if the first word is not "Basic", return unauthorized
	if( string_t::casecmp(words[0], "basic") )	return unauth_err;

	// decode the second parameter which is in base64
	datum_t	decoded = base64_t::decode(words[1]);
	// parse the decoded username:password
	std::vector<std::string>	words2	= string_t::split( string_t::from_datum(decoded), ":", 2);
	// this split MUST contain 2 elements - the first is the username, the second the password
	if( words2.size() != 2 )			return unauth_err;
	// set some alias
	const std::string &	username	= words2[0];
	const std::string &	req_password	= words2[1];
	
	// if the username doesnt exist, return an error
	std::string	user_password;
	if( get_pass_ftor(username, *this, request, user_password) == false )
		return unauth_err;
	// if the req_password doesnt match the user_password, return an error
	if( req_password != user_password )	return unauth_err;

	// if this point is reached, the username/password are allowed, so return httpd_err_t::OK	
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END



/*! \file
    \brief Header of the \ref dnsgrab_register_t class
    
*/


#ifndef __NEOIP_DNSGRAB_REGISTER_HPP__ 
#define __NEOIP_DNSGRAB_REGISTER_HPP__ 
/* system include */
/* local include */
#include "neoip_dnsgrab_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_err.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief define a handler for a specific http path
 */
class dnsgrab_register_t : NEOIP_COPY_CTOR_ALLOW, private timeout_cb_t {
public:	/////////////////////// constant declaration ///////////////////////////
	//! The period at which the config file is rewritten
	// TODO this should in a profile
	static const delay_t	CFGFILE_REWRITE_PERIOD;		
private:
	file_path_t	cfgfile_path;
	std::string	config_str;
	
	/*************** rewrite_timeout	*******************************/
	timeout_t	rewrite_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();

	/*************** Internal function	*******************************/
	file_err_t	write_cfgfile()	throw();
public:
	/*************** ctor/dtor	***************************************/
	dnsgrab_register_t()		throw();
	~dnsgrab_register_t()		throw();

	/*************** setup function	***************************************/
	dnsgrab_err_t	start(const std::string &location, size_t priority
				, const ipport_addr_t &listen_addr, const delay_t &request_ttl
				, const std::string &tag, const std::string &nounce)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DNSGRAB_REGISTER_HPP__  */




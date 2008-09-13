/*! \file
    \brief Header of the \ref kad_http_t class

*/

#ifdef __linux__

#ifndef __NEOIP_HOST2IP_AGAI_HPP__ 
#define __NEOIP_HOST2IP_AGAI_HPP__ 
/* system include */
#include <netdb.h>
/* local include */
#include "neoip_host2ip_cb.hpp"
#include "neoip_host2ip_vapi.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief to perform an host2ip_agai_t using agaihronous gethostbyname
 */
class host2ip_agai_t : NEOIP_COPY_CTOR_DENY, public host2ip_vapi_t, private timeout_cb_t {
private:
	std::string	m_hostname;	//!< the hostname which is queried

	char		glibc_gai_name[1024];	//!<to store the name in a C format
	struct	gaicb	glibc_gai_stt;		//!< glibc structure to store data during getaddrinfo_a
	bool		pending_flag;

	/*************** probe_timeout	***************************************/
	delaygen_t	probe_delaygen;		//!< the delay_t generator for the probing	
	timeout_t	probe_timeout;	
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** callback stuff	***************************************/
	host2ip_cb_t *	callback;	//!< the callback to notify on completion
	void *		userptr;	//!< the userptr associated with the callback
	bool		notify_callback(const inet_err_t &inet_err
					, const std::vector<ip_addr_t> &result)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	host2ip_agai_t()	throw();
	~host2ip_agai_t()	throw();
	
	/*************** setup function	***************************************/
	inet_err_t	start(const std::string &m_hostname, host2ip_cb_t *callback
							, void *userptr)	throw();

	/*************** query function	***************************************/
	const std::string &	hostname()	const throw()	{ return m_hostname;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HOST2IP_AGAI_HPP__  */

#endif // _WIN32

 

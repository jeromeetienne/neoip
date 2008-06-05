/*! \file
    \brief Header of the \ref kad_http_t class

*/


#ifndef __NEOIP_HOST2IP_FORK_HPP__ 
#define __NEOIP_HOST2IP_FORK_HPP__ 
/* system include */
/* local include */
#include "neoip_host2ip_cb.hpp"
#include "neoip_host2ip_vapi.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_asyncexe_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief to perform an host2ip_fork_t using synchronous gethostbyname
 */
class host2ip_fork_t : NEOIP_COPY_CTOR_DENY, private asyncexe_cb_t, public host2ip_vapi_t {
private:
	std::string	m_hostname;	
	/*************** asyncexe	***************************************/
	asyncexe_t *	asyncexe;
	bool		neoip_asyncexe_cb(void *cb_userptr, asyncexe_t &cb_asyncexe
					, const libsess_err_t &libsess_err, const bytearray_t &stdout_barray
					, const int &exit_status)	throw();

	/*************** callback stuff	***************************************/
	host2ip_cb_t *	callback;	//!< the callback to notify on completion
	void *		userptr;	//!< the userptr associated with the callback
	bool		notify_callback_err(const inet_err_t &inet_err)		throw();
	bool		notify_callback(const inet_err_t &inet_err
					, const std::vector<ip_addr_t> &result)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	host2ip_fork_t()	throw();
	~host2ip_fork_t()	throw();
	
	/*************** setup function	***************************************/
	inet_err_t	start(const std::string &m_hostname, host2ip_cb_t *callback
							, void *userptr)	throw();

	/*************** query function	***************************************/
	const std::string &	hostname()	const throw()	{ return m_hostname;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HOST2IP_FORK_HPP__  */


 

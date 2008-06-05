/*! \file
    \brief Header of the \ref kad_http_t class

*/


#ifndef __NEOIP_IPCOUNTRY_HPP__ 
#define __NEOIP_IPCOUNTRY_HPP__ 
/* system include */
/* local include */
#include "neoip_ipcountry_cb.hpp"
#include "neoip_host2ip_cb.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief to perform an ipcountry_t using synchronous gethostbyname
 */
class ipcountry_t : NEOIP_COPY_CTOR_DENY, private host2ip_cb_t {
private:
	ip_addr_t	m_ipaddr;	//!< the ip_addr_t which is queried

	/*************** Internal function	*******************************/
	const char *	ipaddr2countrycode(const ip_addr_t &ip_addr)	const throw();

	/*************** host2ip_t	***************************************/
	host2ip_t *	host2ip;
	bool		neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
						, const inet_err_t &inet_err
						, const std::vector<ip_addr_t> &result_arr)	throw();
	/*************** callback stuff	***************************************/
	ipcountry_cb_t *callback;	//!< the callback to notify on completion
	void *		userptr;	//!< the userptr associated with the callback
	bool		notify_callback(const inet_err_t &inet_err, const std::string &country_code)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	ipcountry_t()	throw();
	~ipcountry_t()	throw();
	
	/*************** setup function	***************************************/
	inet_err_t	start(const ip_addr_t &m_ipaddr, ipcountry_cb_t *callback, void *userptr) throw();

	/*************** query function	***************************************/
	const ip_addr_t &	ipaddr()	const throw()	{ return m_ipaddr;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPCOUNTRY_HPP__  */


 

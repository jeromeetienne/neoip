/*! \file
    \brief Header of the host2ip_t callback class
    
*/


#ifndef __NEOIP_HOST2IP_CB_HPP__ 
#define __NEOIP_HOST2IP_CB_HPP__ 
/* system include */
#include <vector>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	host2ip_t;
class	host2ip_vapi_t;
class	ip_addr_t;
class	inet_err_t;
	
/** \brief the callback class for host2ip_t
 */
class host2ip_cb_t {
public:
	/** \brief callback notified by host2ip_t when the result is known
	 * 
	 * - TODO why does this return a std::vector and not a item_arr_t ?
	 *   - i dont see any good reason for that right now
	 *   - if confirmed, change it to item_arr_t
	 */
	virtual bool neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
				, const inet_err_t &inet_err
				, const std::vector<ip_addr_t> &ipaddr_arr)	throw() = 0;
	virtual ~host2ip_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HOST2IP_CB_HPP__  */




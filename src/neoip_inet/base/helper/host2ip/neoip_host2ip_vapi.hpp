/*! \file
    \brief Header of the host2ip_t virtual API class
    
*/


#ifndef __NEOIP_HOST2IP_VAPI_HPP__
#define __NEOIP_HOST2IP_VAPI_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;
/** \brief the virtual api class for host2ip_t
 */
class host2ip_vapi_t {
public:
	// return the queried hostname
	virtual const std::string &	hostname()	const throw() = 0;
	// virtual destructor
	virtual ~host2ip_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HOST2IP_VAPI_HPP__  */




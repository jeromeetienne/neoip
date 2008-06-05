/*! \file
    \brief Declaration of dns_helper_t
    
*/


#ifndef __NEOIP_DNS_HELPER_HPP__ 
#define __NEOIP_DNS_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief helper to check dns name
 */
class dns_helper_t {
public:
	static bool		label_is_valid(const std::string &label)	throw();
	static std::string	idna_to_ascii(const std::string &idna_str)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DNS_HELPER_HPP__  */




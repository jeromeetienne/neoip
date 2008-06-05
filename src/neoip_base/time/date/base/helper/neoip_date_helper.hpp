/*! \file
    \brief Declaration of static helper functions for date_t
    
*/


#ifndef __NEOIP_DATE_HELPER_HPP__ 
#define __NEOIP_DATE_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_date.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief static helpers for date_t
 */
class date_helper_t {
public:
	static std::string	to_canonical_string(const date_t &date)		throw();
	static date_t		from_canonical_string(const std::string &str)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DATE_HELPER_HPP__  */











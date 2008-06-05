/*! \file
    \brief Header of the neoip_date
    
*/


#ifndef __NEOIP_DATE_HPP__ 
#define __NEOIP_DATE_HPP__ 
/* system include */
#include <sys/time.h>
/* local include */
#include "neoip_delay.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Handle date (aka a given point in time)
 * 
 * - it inherit of delay_t
 */
class date_t : public delay_t {
public:
	/*************** ctor/dtor	***************************************/
	date_t(const delay_t &delay)		throw()	: delay_t(delay) {}
	date_t()				throw() {}

	/*************** query function	***************************************/
	static date_t		present()	throw();
	bool 			is_future()	const throw();
	bool 			is_past()	const throw();

	/*************** convertion function	*******************************/	
	time_t			to_time_t()	const throw();
	std::string		to_ctime()	const throw();
	std::string		to_string()	const throw();
	std::string		to_canonical_string()				const throw();
	static date_t		from_canonical_string(const std::string &str)	throw();

	/*************** display function	*******************************/
	friend	std::ostream& operator << (std::ostream& os, const date_t & date)	throw()
					{ return os << date.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DATE_HPP__  */




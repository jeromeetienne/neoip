/*! \file
    \brief Header of the \ref reachpeer_profile_t

- see \ref neoip_reachpeer_profile.cpp
*/


#ifndef __NEOIP_REACHPEER_PROFILE_HPP__ 
#define __NEOIP_REACHPEER_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_reachpeer_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class reachpeer_profile_t : NEOIP_COPY_CTOR_ALLOW {
private:
	delay_t		silent_delay_val;
	delay_t		unreach_delay_val;
	delay_t		probe_period_val;
public:
	// ctor/dtor
	reachpeer_profile_t()	throw();
	~reachpeer_profile_t()	throw();

	// get/set parameter
	const delay_t &	silent_delay()	const throw()	{ return silent_delay_val;	};
	delay_t &	silent_delay()	throw()		{ return silent_delay_val;	};
	const delay_t &	unreach_delay()	const throw()	{ return unreach_delay_val;	};
	delay_t &	unreach_delay()	throw()		{ return unreach_delay_val;	};
	const delay_t &	probe_period()	const throw()	{ return probe_period_val;	};
	delay_t &	probe_period()	throw()		{ return probe_period_val;	};
	
	// check() function
	reachpeer_err_t	check()		const throw();
	
	// display functions
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const reachpeer_profile_t &profile)
			{ return os << profile.to_string();	}	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_REACHPEER_PROFILE_HPP__  */




/*! \file
    \brief Header of the \ref nlay_idletimeout_profile_t

- see \ref neoip_idletimeout_profile.cpp
*/


#ifndef __NEOIP_IDLETIMEOUT_PROFILE_HPP__ 
#define __NEOIP_IDLETIMEOUT_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_delay.hpp"
#include "neoip_nlay_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile of the \ref nlay_idletimeout_t
 */
class nlay_idletimeout_profile_t : NEOIP_COPY_CTOR_ALLOW {
private:
	delay_t		idle_delay_val;
public:
	// ctor/dtor
	nlay_idletimeout_profile_t()	throw();
	~nlay_idletimeout_profile_t()	throw();

	// get/set parameter
	const delay_t &	idle_delay()	const throw()	{ return idle_delay_val;	};
	delay_t &	idle_delay()	throw()		{ return idle_delay_val;	};
	
	// check() function
	nlay_err_t	check()		const throw();
	
	// display functions
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const nlay_idletimeout_profile_t &profile)
			{ return os << profile.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IDLETIMEOUT_PROFILE_HPP__  */




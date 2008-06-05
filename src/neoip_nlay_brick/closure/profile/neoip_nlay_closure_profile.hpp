/*! \file
    \brief Header of the \ref nlay_closure_profile_t

- see \ref neoip_closure_profile.cpp
*/


#ifndef __NEOIP_CLOSURE_PROFILE_HPP__ 
#define __NEOIP_CLOSURE_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_nlay_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile of the \ref nlay_closure_t
 */
class nlay_closure_profile_t : NEOIP_COPY_CTOR_ALLOW {
private:
	delay_t		close_itor_delay_val;
	delay_t		close_resp_delay_val;
	delay_t		probe_period_val;
public:
	// ctor/dtor
	nlay_closure_profile_t()	throw();
	~nlay_closure_profile_t()	throw();

	// get/set parameter
	const delay_t &	close_itor_delay()	const throw()	{ return close_itor_delay_val;	};
	delay_t &	close_itor_delay()	throw()		{ return close_itor_delay_val;	};
	const delay_t &	close_resp_delay()	const throw()	{ return close_resp_delay_val;	};
	delay_t &	close_resp_delay()	throw()		{ return close_resp_delay_val;	};
	const delay_t &	probe_period()		const throw()	{ return probe_period_val;	};
	delay_t &	probe_period()		throw()		{ return probe_period_val;	};
	
	// check() function
	nlay_err_t	check()		const throw();
	
	// display functions
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const nlay_closure_profile_t &profile)
			{ return os << profile.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CLOSURE_PROFILE_HPP__  */




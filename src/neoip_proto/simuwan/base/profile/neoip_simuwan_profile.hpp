/*! \file
    \brief Header of the \ref simuwan_profile_t

- see \ref neoip_simuwan_profile.cpp
*/


#ifndef __NEOIP_SIMUWAN_PROFILE_HPP__ 
#define __NEOIP_SIMUWAN_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_simuwan_err.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class simuwan_profile_t : NEOIP_COPY_CTOR_ALLOW {
private:
	double		drop_rate_val;
	double		delayed_rate_val;
	delay_t		delayed_min_val;
	delay_t		delayed_max_val;
	double		duplicate_rate_val;
	size_t		duplicate_min_val;
	size_t		duplicate_max_val;
public:
	// ctor/dtor
	simuwan_profile_t()	throw();
	~simuwan_profile_t()	throw();

	// get/set parameter
	double &	drop_rate()	throw()		{ return drop_rate_val;		};
	const double &	drop_rate()	const throw()	{ return drop_rate_val;		};
	double &	delayed_rate()	throw()		{ return delayed_rate_val;	};
	const double &	delayed_rate()	const throw()	{ return delayed_rate_val;	};
	delay_t &	delayed_min()	throw()		{ return delayed_min_val;	};
	const delay_t &	delayed_min()	const throw()	{ return delayed_min_val;	};
	delay_t &	delayed_max()	throw()		{ return delayed_max_val;	};
	const delay_t &	delayed_max()	const throw()	{ return delayed_max_val;	};
	double &	duplicate_rate()throw()		{ return duplicate_rate_val;	};
	const double &	duplicate_rate()const throw()	{ return duplicate_rate_val;	};
	size_t &	duplicate_min()	throw()		{ return duplicate_min_val;	};
	const size_t &	duplicate_min()	const throw()	{ return duplicate_min_val;	};
	size_t &	duplicate_max()	throw()		{ return duplicate_max_val;	};
	const size_t &	duplicate_max()	const throw()	{ return duplicate_max_val;	};
	
	// check() function
	simuwan_err_t	check()		const throw();
	
	// display functions
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const simuwan_profile_t &profile)
			{ return os << profile.to_string();	}	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SIMUWAN_PROFILE_HPP__  */




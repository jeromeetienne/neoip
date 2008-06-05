/*! \file
    \brief Header of the \ref nlay_rate_limit_profile_t

- see \ref neoip_rate_limit_profile.cpp
*/


#ifndef __NEOIP_NLAY_RATE_LIMIT_PROFILE_HPP__ 
#define __NEOIP_NLAY_RATE_LIMIT_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_delay.hpp"
#include "neoip_nlay_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the profile of the \ref nlay_rate_limit_t
 */
class nlay_rate_limit_profile_t : NEOIP_COPY_CTOR_ALLOW {
private:
	uint64_t	xmit_max_throughput_val;
	delay_t		xmit_estim_range_val;
	delay_t		xmit_estim_granularity_val;
	uint64_t	recv_max_throughput_val;
	delay_t		recv_estim_range_val;
	delay_t		recv_estim_granularity_val;
public:
	// ctor/dtor
	nlay_rate_limit_profile_t()	throw();
	~nlay_rate_limit_profile_t()	throw();

	// get/set parameter
	const uint64_t &xmit_max_throughput()	const throw()	{ return xmit_max_throughput_val;	};
	uint64_t &	xmit_max_throughput()	throw()		{ return xmit_max_throughput_val;	};
	const delay_t &	xmit_estim_range()	const throw()	{ return xmit_estim_range_val;		};
	delay_t &	xmit_estim_range()	throw()		{ return xmit_estim_range_val;		};
	const delay_t &	xmit_estim_granularity()const throw()	{ return xmit_estim_granularity_val;	};
	delay_t &	xmit_estim_granularity()throw()		{ return xmit_estim_granularity_val;	};
	const uint64_t &recv_max_throughput()	const throw()	{ return recv_max_throughput_val;	};
	uint64_t &	recv_max_throughput()	throw()		{ return recv_max_throughput_val;	};
	const delay_t &	recv_estim_range()	const throw()	{ return recv_estim_range_val;		};
	delay_t &	recv_estim_range()	throw()		{ return recv_estim_range_val;		};
	const delay_t &	recv_estim_granularity()const throw()	{ return recv_estim_granularity_val;	};
	delay_t &	recv_estim_granularity()throw()		{ return recv_estim_granularity_val;	};
	
	// check() function
	nlay_err_t	check()		const throw();
	
	// display functions
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const nlay_rate_limit_profile_t &profile)
			{ return os << profile.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_RATE_LIMIT_PROFILE_HPP__  */




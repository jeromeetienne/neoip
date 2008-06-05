/*! \file
    \brief Definition of the \ref nlay_rate_limit_profile_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_nlay_rate_limit_profile.hpp"
#include "neoip_nlay_rate_limit.hpp"

NEOIP_NAMESPACE_BEGIN;


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_rate_limit_profile_t::nlay_rate_limit_profile_t()	throw()
{
	// set the default parameter
	xmit_max_throughput_val		= nlay_rate_limit_t::XMIT_MAX_THROUGHPUT_DFL;
	xmit_estim_range_val		= nlay_rate_limit_t::XMIT_ESTIM_RANGE_DFL;
	xmit_estim_granularity_val	= nlay_rate_limit_t::XMIT_ESTIM_GRANULARITY_DFL;
	recv_max_throughput_val		= nlay_rate_limit_t::RECV_MAX_THROUGHPUT_DFL;
	recv_estim_range_val		= nlay_rate_limit_t::RECV_ESTIM_RANGE_DFL;
	recv_estim_granularity_val	= nlay_rate_limit_t::RECV_ESTIM_GRANULARITY_DFL;
}

/** \brief Destructor
 */
nlay_rate_limit_profile_t::~nlay_rate_limit_profile_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     to_string() function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Convert the object into a string
 */
std::string	nlay_rate_limit_profile_t::to_string()	const throw()
{
	std::ostringstream oss;
	oss << "xmit_max_throughput="	<< xmit_max_throughput() << " per sec";
	oss << " estimated over "	<< xmit_estim_range();
	oss << " with a granularity of "<< xmit_estim_granularity();
	oss << ". recv_max_throughput=" << recv_max_throughput_val << " per sec";
	oss << " estimated over "	<< recv_estim_range();
	oss << " with a granularity of "<< recv_estim_granularity();
	return oss.str();
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 * 
 * @return false if there is no error in the profile, true otherwise
 */
nlay_err_t	nlay_rate_limit_profile_t::check()	const throw()
{
	return nlay_err_t::OK;
}

NEOIP_NAMESPACE_END


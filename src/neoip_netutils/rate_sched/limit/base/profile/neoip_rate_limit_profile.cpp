/*! \file
    \brief Definition of the \ref rate_limit_profile_t

*/


/* system include */
/* local include */
#include "neoip_rate_limit_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref rate_limit_profile_t constant
const delay_t	rate_limit_profile_t::REQ_WINDOW_DELAY		= delay_t::from_msec(500);
const double	rate_limit_profile_t::REQ_WINDOW_RANDRATE	= 0.3;
const delay_t	rate_limit_profile_t::FROZEN_WINDOW_DELAY	= delay_t::from_msec(1000);
const double	rate_limit_profile_t::FROZEN_WINDOW_RANDRATE	= 0.3;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
rate_limit_profile_t::rate_limit_profile_t()	throw()
{
	req_window_delay	(REQ_WINDOW_DELAY);
	req_window_randrate	(REQ_WINDOW_RANDRATE);
	frozen_window_delay	(FROZEN_WINDOW_DELAY);
	frozen_window_randrate	(FROZEN_WINDOW_RANDRATE);
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
rate_err_t	rate_limit_profile_t::check()	const throw()
{
	// return no error
	return rate_err_t::OK;
}

NEOIP_NAMESPACE_END


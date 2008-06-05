/*! \file
    \brief Definition of the \ref upnp_portcleaner_profile_t

*/


/* system include */
/* local include */
#include "neoip_upnp_portcleaner_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref upnp_portcleaner_profile_t constant
#if 1
	const delaygen_arg_t	upnp_portcleaner_profile_t::PROBE_DELAYGEN_ARG	= delaygen_regular_arg_t()
							.period(delay_t::from_min(5))
							.random_range(0.2);
#else
	const delaygen_arg_t	upnp_portcleaner_profile_t::PROBE_DELAYGEN_ARG	= delaygen_regular_arg_t()
							.period(delay_t::from_sec(5))
							.random_range(0.2);
#endif

// end of constants definition
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_portcleaner_profile_t::upnp_portcleaner_profile_t()	throw()
{
	probe_delaygen_arg	(PROBE_DELAYGEN_ARG);
}

/** \brief Destructor
 */
upnp_portcleaner_profile_t::~upnp_portcleaner_profile_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
upnp_err_t	upnp_portcleaner_profile_t::check()	const throw()
{
	// return no error
	return upnp_err_t::OK;
}

NEOIP_NAMESPACE_END


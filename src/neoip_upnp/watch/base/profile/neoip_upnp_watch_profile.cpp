/*! \file
    \brief Definition of the \ref upnp_watch_profile_t

*/


/* system include */
/* local include */
#include "neoip_upnp_watch_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref upnp_watch_profile_t constant
#if 1
	const delaygen_arg_t	upnp_watch_profile_t::DISC_DELAYGEN_ARG		= delaygen_regular_arg_t()
							.period(delay_t::from_min(10))
							.random_range(0.2);
	const delaygen_arg_t	upnp_watch_profile_t::EXTIPADDR_DELAYGEN_ARG	= delaygen_regular_arg_t()
							.period(delay_t::from_min(3))
							.random_range(0.2);
	const bool		upnp_watch_profile_t::PORTCLEANER_ENABLED	= true;
#else
	const delaygen_arg_t	upnp_watch_profile_t::DISC_DELAYGEN_ARG		= delaygen_regular_arg_t()
							.period(delay_t::from_sec(60))
							.random_range(0.2);
	const delaygen_arg_t	upnp_watch_profile_t::EXTIPADDR_DELAYGEN_ARG	= delaygen_regular_arg_t()
							.period(delay_t::from_sec(60))
							.random_range(0.2);
	const bool		upnp_watch_profile_t::PORTCLEANER_ENABLED	= true;
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_watch_profile_t::upnp_watch_profile_t()	throw()
{
	disc_delaygen_arg	(DISC_DELAYGEN_ARG	);
	extipaddr_delaygen_arg	(EXTIPADDR_DELAYGEN_ARG	);
	portcleaner_enabled	(PORTCLEANER_ENABLED	);
}

/** \brief Destructor
 */
upnp_watch_profile_t::~upnp_watch_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
upnp_err_t	upnp_watch_profile_t::check()	const throw()
{
	// return no error
	return upnp_err_t::OK;
}

NEOIP_NAMESPACE_END


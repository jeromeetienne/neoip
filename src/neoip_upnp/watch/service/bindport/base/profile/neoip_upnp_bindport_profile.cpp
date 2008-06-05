/*! \file
    \brief Definition of the \ref upnp_bindport_profile_t

*/


/* system include */
/* local include */
#include "neoip_upnp_bindport_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref upnp_bindport_profile_t constant
#if 1
	const delay_t		upnp_bindport_profile_t::ITOR_EXPIRE_DELAY	= delay_t::from_min(2);
	const size_t		upnp_bindport_profile_t::GETPORT_NBRETRY_MAX	= 10;
	const delaygen_arg_t	upnp_bindport_profile_t::ADDPORT_DELAYGEN_ARG	= delaygen_regular_arg_t()
							.period(delay_t::from_min(3))
							.random_range(0.2);
	const delay_t		upnp_bindport_profile_t::PORTCLEANER_LEASE_DELAY= delay_t::from_min(5);
#else
	const delay_t		upnp_bindport_profile_t::ITOR_EXPIRE_DELAY	= delay_t::from_sec(30);
	const size_t		upnp_bindport_profile_t::GETPORT_NBRETRY_MAX	= 5;
	const delaygen_arg_t	upnp_bindport_profile_t::ADDPORT_DELAYGEN_ARG	= delaygen_regular_arg_t()
							.period(delay_t::from_sec(5))
							.random_range(0.2);
	const delay_t		upnp_bindport_profile_t::PORTCLEANER_LEASE_DELAY= delay_t::from_sec(10);
#endif

// end of constants definition
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_bindport_profile_t::upnp_bindport_profile_t()	throw()
{
	itor_expire_delay	( ITOR_EXPIRE_DELAY		);
	getport_nbretry_max	( GETPORT_NBRETRY_MAX		);
	addport_delaygen_arg	( ADDPORT_DELAYGEN_ARG		);
	portcleaner_lease_delay	( PORTCLEANER_LEASE_DELAY	);
}

/** \brief Destructor
 */
upnp_bindport_profile_t::~upnp_bindport_profile_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
upnp_err_t	upnp_bindport_profile_t::check()	const throw()
{
	// TODO to check that addport CAN NOT be higher than portcleaner_lease_delay
	// return no error
	return upnp_err_t::OK;
}

NEOIP_NAMESPACE_END


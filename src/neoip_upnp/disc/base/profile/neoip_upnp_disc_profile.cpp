/*! \file
    \brief Definition of the \ref upnp_disc_profile_t

*/


/* system include */
/* local include */
#include "neoip_upnp_disc_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref upnp_disc_profile_t constant
const delaygen_arg_t	upnp_disc_profile_t::NUDPRXMIT_DELAYGEN_ARG	= delaygen_regular_arg_t()
							.period(delay_t::from_sec(5))
							.random_range(0.2);
const delay_t		upnp_disc_profile_t::EXPIRE_DELAY		= delay_t::from_min(3);
const bool		upnp_disc_profile_t::GETPORTENDIAN_TEST_ENABLED	= true;

// end of constants definition
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_disc_profile_t::upnp_disc_profile_t()	throw()
{
	nudprxmit_delaygen_arg		( NUDPRXMIT_DELAYGEN_ARG	);
	expire_delay			( EXPIRE_DELAY			);
	getportendian_test_enabled	( GETPORTENDIAN_TEST_ENABLED	);
}

/** \brief Destructor
 */
upnp_disc_profile_t::~upnp_disc_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
upnp_err_t	upnp_disc_profile_t::check()	const throw()
{
	// return no error
	return upnp_err_t::OK;
}

NEOIP_NAMESPACE_END


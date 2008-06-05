/*! \file
    \brief Definition of the \ref upnp_call_profile_t

- TODO the PORTCLEANER_ENABLED is used only in the upnp_call_addport_t
  - this should be in a separate profile dedicated to upnp_call_addport_t
  - not here. this breaks the unicity rule

*/


/* system include */
/* local include */
#include "neoip_upnp_call_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref upnp_call_profile_t constant
const delay_t		upnp_call_profile_t::EXPIRE_DELAY		= delay_t::from_sec(20);
const bool		upnp_call_profile_t::PORTCLEANER_ENABLED	= true;
const delaygen_arg_t	upnp_call_profile_t::HTTPERR_RETRY_DELAYGEN	= delaygen_expboff_arg_t()
							.first_delay(delay_t::from_sec(1))
							.min_delay(delay_t::from_sec(1))
							.max_delay(delay_t::from_sec(4))
							.random_range(0.2);
const size_t		upnp_call_profile_t::HTTPERR_RETRY_NBMAX	= 10;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_call_profile_t::upnp_call_profile_t()	throw()
{
	// set the constant fields
	expire_delay		( EXPIRE_DELAY		);
	portcleaner_enabled	( PORTCLEANER_ENABLED	);
	httperr_retry_delaygen	( HTTPERR_RETRY_DELAYGEN);
	httperr_retry_nbmax	( HTTPERR_RETRY_NBMAX	);
}

/** \brief Destructor
 */
upnp_call_profile_t::~upnp_call_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
upnp_err_t	upnp_call_profile_t::check()	const throw()
{
	// return no error
	return upnp_err_t::OK;
}

NEOIP_NAMESPACE_END


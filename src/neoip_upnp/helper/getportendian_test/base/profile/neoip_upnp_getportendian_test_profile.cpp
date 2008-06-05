/*! \file
    \brief Definition of the \ref upnp_getportendian_test_profile_t

*/


/* system include */
/* local include */
#include "neoip_upnp_getportendian_test_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref upnp_getportendian_test_profile_t constant
const delay_t	upnp_getportendian_test_profile_t::EXPIRE_DELAY		= delay_t::from_min(3);
const size_t	upnp_getportendian_test_profile_t::GETPORT_NBRETRY_MAX	= 5;
// end of constants definition

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_getportendian_test_profile_t::upnp_getportendian_test_profile_t()	throw()
{
	expire_delay		(EXPIRE_DELAY);
	getport_nbretry_max	(GETPORT_NBRETRY_MAX);
}

/** \brief Destructor
 */
upnp_getportendian_test_profile_t::~upnp_getportendian_test_profile_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
upnp_err_t	upnp_getportendian_test_profile_t::check()	const throw()
{
	// return no error
	return upnp_err_t::OK;
}

NEOIP_NAMESPACE_END


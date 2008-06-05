/*! \file
    \brief Definition of the \ref ndiag_watch_profile_t

- TODO split this ndiag_watch_profile_t into several sub one
  - e.g. there are 3 field only for itor

*/


/* system include */
/* local include */
#include "neoip_ndiag_watch_profile.hpp"

NEOIP_NAMESPACE_BEGIN;


// definition of \ref ndiag_watch_profile_t constant
#if 0	// TODO not sure this is still in use .... neoip_session.conf got a var for that
	const bool	ndiag_watch_profile_t::ENABLE_UPNP_WATCH	= true;
#else
	const bool	ndiag_watch_profile_t::ENABLE_UPNP_WATCH	= false;
#endif
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ndiag_watch_profile_t::ndiag_watch_profile_t()	throw()
{
	// set the constant fields
	enable_upnp_watch	(ENABLE_UPNP_WATCH);
		
	// sanity check - the profile MUST succeed check() by default
	DBG_ASSERT( check().succeed() );
}
/** \brief Destructor
 */
ndiag_watch_profile_t::~ndiag_watch_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ndiag_err_t	ndiag_watch_profile_t::check()	const throw()
{
	// return no error
	return ndiag_err_t::OK;
}

NEOIP_NAMESPACE_END


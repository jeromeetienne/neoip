/*! \file
    \brief Definition of the \ref pktfrag_profile_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_pktfrag_profile.hpp"
#include "neoip_pktfrag.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
pktfrag_profile_t::pktfrag_profile_t()	throw()
{
	// set the default parameter
	outter_mtu		( pktfrag_t::OUTTER_MTU_DFL );
	pending_dgram_expire	( pktfrag_t::PENDING_DGRAM_EXPIRE_DFL );
	pool_max_size		( pktfrag_t::POOL_MAX_SIZE_DFL );
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
pktfrag_err_t	pktfrag_profile_t::check()	const throw()
{
	// return noerror
	return pktfrag_err_t::OK;
}

NEOIP_NAMESPACE_END


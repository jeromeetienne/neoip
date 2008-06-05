/*! \file
    \brief Definition of the \ref tcp_itor_profile_t

*/


/* system include */
/* local include */
#include "neoip_tcp_itor_profile.hpp"
#include "neoip_tcp_itor.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref tcp_itor_profile_t constant
const delay_t	tcp_itor_profile_t::TIMEOUT_DELAY	= delay_t::from_sec(3*60);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
tcp_itor_profile_t::tcp_itor_profile_t()	throw()
{
	timeout_delay	(TIMEOUT_DELAY);
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
inet_err_t	tcp_itor_profile_t::check()	const throw()
{
	// return no error
	return inet_err_t::OK;
}

NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the \ref kad_nsearch_profile_t

*/


/* system include */
/* local include */
#include "neoip_kad_nsearch_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_nsearch_profile_t constant
const delay_t	kad_nsearch_profile_t::RPCCLI_SOFT_TIMEOUT	= delay_t::from_sec(60);
const delay_t	kad_nsearch_profile_t::RPCCLI_HARD_TIMEOUT	= delay_t::from_sec(60);
const size_t	kad_nsearch_profile_t::NB_CONCURRENT_REQ	= 3;
const bool	kad_nsearch_profile_t::CACHING_ENABLE		= true;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_nsearch_profile_t::kad_nsearch_profile_t()	throw()
{
	rpccli_soft_timeout(	RPCCLI_SOFT_TIMEOUT);
	rpccli_hard_timeout(	RPCCLI_HARD_TIMEOUT);
	nb_concurrent_req(	NB_CONCURRENT_REQ);
	caching_enable(		CACHING_ENABLE);
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
kad_err_t	kad_nsearch_profile_t::check()	const throw()
{
	// return no error
	return kad_err_t::OK;
}

NEOIP_NAMESPACE_END


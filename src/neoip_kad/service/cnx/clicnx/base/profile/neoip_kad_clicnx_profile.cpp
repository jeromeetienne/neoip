/*! \file
    \brief Definition of the \ref kad_clicnx_profile_t

*/


/* system include */
/* local include */
#include "neoip_kad_clicnx_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_clicnx_profile_t constant
const delaygen_arg_t	kad_clicnx_profile_t::RXMIT_DELAYGEN_ARG = delaygen_expboff_arg_t()
								.min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(20))
								.timeout_delay(delay_t::from_sec(60))
								.random_range(0.2);
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_clicnx_profile_t::kad_clicnx_profile_t()	throw()
{
	rxmit_delaygen_arg(RXMIT_DELAYGEN_ARG);
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
kad_err_t	kad_clicnx_profile_t::check()	const throw()
{
	// return no error
	return kad_err_t::OK;
}

NEOIP_NAMESPACE_END


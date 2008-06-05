/*! \file
    \brief Definition of the \ref ntudp_npos_eval_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_npos_eval_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_npos_eval_profile_t constant
const delay_t	ntudp_npos_eval_profile_t::EXPIRE_DELAY			= delay_t::from_sec(3*60);
const size_t	ntudp_npos_eval_profile_t::MAX_CONCURRENT_UNKNOWN	= 4;
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_npos_eval_profile_t::ntudp_npos_eval_profile_t()	throw()
{
	expire_delay(EXPIRE_DELAY);
	max_concurrent_unknown(MAX_CONCURRENT_UNKNOWN);
}

/** \brief Destructor
 */
ntudp_npos_eval_profile_t::~ntudp_npos_eval_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_npos_eval_profile_t::check()	const throw()
{
	ntudp_err_t	ntudp_err;
	// check the ntudp_npos_cli_profile_t
	ntudp_err	= cli().check();
	if( ntudp_err.failed() )	return ntudp_err;	
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


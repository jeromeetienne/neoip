/*! \file
    \brief Definition of the \ref ntudp_npos_watch_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_npos_watch_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_npos_watch_profile_t constant
const delaygen_arg_t	ntudp_npos_watch_profile_t::EVAL_TRIGGER	= delaygen_regular_arg_t()
									.period(delay_t::from_sec(5*60));
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_npos_watch_profile_t::ntudp_npos_watch_profile_t()	throw()
{
	eval_trigger(EVAL_TRIGGER);
}

/** \brief Destructor
 */
ntudp_npos_watch_profile_t::~ntudp_npos_watch_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_npos_watch_profile_t::check()	const throw()
{
	ntudp_err_t	ntudp_err;
	// check the ntudp_npos_eval_profile_t
	ntudp_err	= eval().check();
	if( ntudp_err.failed() )	return ntudp_err;	
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


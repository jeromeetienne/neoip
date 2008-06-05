/*! \file
    \brief Definition of the \ref ntudp_pserver_pool_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_pserver_pool_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_pserver_pool_profile_t constant
const delay_t	ntudp_pserver_pool_profile_t::REACH_EXPIRE		= delay_t::from_sec(2*60);
const delay_t	ntudp_pserver_pool_profile_t::UNREACH_EXPIRE		= delay_t::from_sec(5*60);
const delay_t	ntudp_pserver_pool_profile_t::UNREACH_MIN_TIMEOUT	= delay_t::from_sec(60);
const delaygen_arg_t	ntudp_pserver_pool_profile_t::NOQUERY_DELAYGEN	= delaygen_expboff_arg_t()
								.min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(10*60))
								.multiplicator(4)
								.random_range(0.2);
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_pserver_pool_profile_t::ntudp_pserver_pool_profile_t()	throw()
{
	reach_expire(REACH_EXPIRE);
	unreach_expire(UNREACH_EXPIRE);
	unreach_min_timeout(UNREACH_MIN_TIMEOUT);
	noquery_delaygen(NOQUERY_DELAYGEN);
}

/** \brief Destructor
 */
ntudp_pserver_pool_profile_t::~ntudp_pserver_pool_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_pserver_pool_profile_t::check()	const throw()
{
	// the noquery_delaygen MUST NOT have a determined timeout_delay()
	if( noquery_delaygen().timeout_delay().is_special() == false ){
		std::string reason ="ntudp_pserver_pool_profile_t noquery_delaygen MUST NOT have a timeout_delay";
		return ntudp_err_t(ntudp_err_t::ERROR, reason);
	}
	
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the \ref ntudp_itor_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_itor_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_itor_profile_t constant
const delaygen_arg_t	ntudp_itor_profile_t::ITOR_PKT_RXMIT	= delaygen_expboff_arg_t()
									.min_delay(delay_t::from_sec(1))
									.max_delay(delay_t::from_sec(20))
									.random_range(0.2);
const delay_t		ntudp_itor_profile_t::EXPIRE_DELAY	= delay_t::from_sec(60);
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_itor_profile_t::ntudp_itor_profile_t()	throw()
{
	itor_pkt_rxmit(ITOR_PKT_RXMIT);
	expire_delay(EXPIRE_DELAY);
}

/** \brief Destructor
 */
ntudp_itor_profile_t::~ntudp_itor_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_itor_profile_t::check()	const throw()
{
	// the itor_pkt_rxmit delaygen MUST NOT have a determined timeout_delay()
	if( itor_pkt_rxmit().timeout_delay().is_special() == false ){
		std::string	reason ="ntudp_itor_profile_t itor_pkt_rxmit MUST NOT have a timeout_delay";
		return ntudp_err_t(ntudp_err_t::ERROR, reason);
	}
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


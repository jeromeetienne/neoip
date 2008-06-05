/*! \file
    \brief Definition of the \ref ntudp_resp_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_resp_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_resp_profile_t constant
const delaygen_arg_t	ntudp_resp_profile_t::ITOR_PKT_RXMIT	= delaygen_expboff_arg_t()
								.min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(20))
								.random_range(0.2)
								.timeout_delay(delay_t::from_sec(60));
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_resp_profile_t::ntudp_resp_profile_t()	throw()
{
	itor_pkt_rxmit(ITOR_PKT_RXMIT);
}

/** \brief Destructor
 */
ntudp_resp_profile_t::~ntudp_resp_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_resp_profile_t::check()	const throw()
{
	// the itor_pkt_rxmit delaygen MUST have a determined timeout_delay()
	if( itor_pkt_rxmit().timeout_delay().is_special() ){
		std::string	reason ="resp_profile_t itor_pkt_rxmit MUST have a timeout_delay";
		return ntudp_err_t(ntudp_err_t::ERROR, reason);
	}
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the \ref ntudp_relpeer_tunnel_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_relpeer_tunnel_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_relpeer_tunnel_profile_t constant
const delaygen_arg_t	ntudp_relpeer_tunnel_profile_t::ITOR_PKT_RXMIT	= delaygen_expboff_arg_t()
								.min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(20))
								.timeout_delay(delay_t::from_sec(60));
const delaygen_arg_t	ntudp_relpeer_tunnel_profile_t::FULL_PKT_RXMIT	= delaygen_expboff_arg_t()
								.first_delay(delay_t::from_sec(3*60))
								.min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(20))
								.timeout_delay(delay_t::from_sec(3*60+60));
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_relpeer_tunnel_profile_t::ntudp_relpeer_tunnel_profile_t()	throw()
{
	itor_pkt_rxmit(ITOR_PKT_RXMIT);
	full_pkt_rxmit(FULL_PKT_RXMIT);
}

/** \brief Destructor
 */
ntudp_relpeer_tunnel_profile_t::~ntudp_relpeer_tunnel_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_relpeer_tunnel_profile_t::check()	const throw()
{
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


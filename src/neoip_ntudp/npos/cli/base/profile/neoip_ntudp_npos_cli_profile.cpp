/*! \file
    \brief Definition of the \ref ntudp_npos_cli_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_npos_cli_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_npos_cli_profile_t constant
const size_t		ntudp_npos_cli_profile_t::MAX_RECVED_INETREACH_REPLY	= 3;
const delaygen_arg_t	ntudp_npos_cli_profile_t::CLI_ITOR_PKT_RXMIT		= delaygen_expboff_arg_t()
								.min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(20))
								.random_range(0.2)
								.timeout_delay(delay_t::from_sec(60));
const delaygen_arg_t	ntudp_npos_cli_profile_t::CLI_FULL_PKT_RXMIT		= delaygen_expboff_arg_t()
								.first_delay(delay_t::from_sec(3*60))
								.min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(20))
								.random_range(0.2)
								.timeout_delay(delay_t::from_sec(3*60+60));
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_npos_cli_profile_t::ntudp_npos_cli_profile_t()	throw()
{
	max_recved_inetreach_reply	(MAX_RECVED_INETREACH_REPLY);
	cli_itor_pkt_rxmit		(CLI_ITOR_PKT_RXMIT);
	cli_full_pkt_rxmit		(CLI_FULL_PKT_RXMIT);
}

/** \brief Destructor
 */
ntudp_npos_cli_profile_t::~ntudp_npos_cli_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_npos_cli_profile_t::check()	const throw()
{
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


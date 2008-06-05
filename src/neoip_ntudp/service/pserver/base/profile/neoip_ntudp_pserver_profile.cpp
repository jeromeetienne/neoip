/*! \file
    \brief Definition of the \ref ntudp_pserver_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_pserver_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_pserver_profile_t constant
const delay_t	ntudp_pserver_profile_t::EXTCNX_EXPIRE		= delay_t::from_sec(60);
const delay_t	ntudp_pserver_profile_t::TUNNEL_IDLE_TIMEOUT	= delay_t::from_sec(4*60);
const size_t	ntudp_pserver_profile_t::TUNNEL_NB_CLOSURE_PKT	= 3;
const delay_t	ntudp_pserver_profile_t::PSERVER_RECORD_TTL	= delay_t::from_sec(3*60);
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_pserver_profile_t::ntudp_pserver_profile_t()	throw()
{
	extcnx_expire		(EXTCNX_EXPIRE);
	tunnel_idle_timeout	(TUNNEL_IDLE_TIMEOUT);
	tunnel_nb_closure_pkt	(TUNNEL_NB_CLOSURE_PKT);
	pserver_record_ttl	(PSERVER_RECORD_TTL);
}

/** \brief Destructor
 */
ntudp_pserver_profile_t::~ntudp_pserver_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_pserver_profile_t::check()	const throw()
{
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the \ref ntudp_peer_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_peer_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_peer_profile_t constant
const std::string	ntudp_peer_profile_t::PSERVER_KEY_STR		= "ntudp pserver record key";	
const size_t		ntudp_peer_profile_t::NB_NEEDED_AVIEW_INIT	= 1;
const delay_t		ntudp_peer_profile_t::PEER_RECORD_TTL		= delay_t::from_sec(3*60);
const bool		ntudp_peer_profile_t::NO_NPOS_WATCH		= false;
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_peer_profile_t::ntudp_peer_profile_t()	throw()
{
	pserver_key_str		(PSERVER_KEY_STR);
	nb_needed_aview_init	(NB_NEEDED_AVIEW_INIT);
	peer_record_ttl		(PEER_RECORD_TTL);
	no_npos_watch		(NO_NPOS_WATCH);
}

/** \brief Destructor
 */
ntudp_peer_profile_t::~ntudp_peer_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_peer_profile_t::check()	const throw()
{
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


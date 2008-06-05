/*! \file
    \brief Definition of the \ref ntudp_relpeer_profile_t

*/


/* system include */
/* local include */
#include "neoip_ntudp_relpeer_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref ntudp_relpeer_profile_t constant
const size_t		ntudp_relpeer_profile_t::NB_NEEDED_TUNNEL	= 1;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_relpeer_profile_t::ntudp_relpeer_profile_t()	throw()
{
	nb_needed_tunnel(NB_NEEDED_TUNNEL);

}
/** \brief Destructor
 */
ntudp_relpeer_profile_t::~ntudp_relpeer_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_relpeer_profile_t::check()	const throw()
{
	ntudp_err_t	ntudp_err;
	// check the ntudp_relpeer_tunnel_profile_t
	ntudp_err	= tunnel().check();
	if( ntudp_err.failed() )	return ntudp_err;
	// check the nb_needed_tunnel
	if(nb_needed_tunnel() == 0)	return ntudp_err_t::ERROR;
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the \ref socket_linger_profile_t

*/


/* system include */
/* local include */
#include "neoip_socket_linger_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref socket_linger_profile_t constant
#if 1	// default value
	const bool	socket_linger_profile_t::ONOFF		= false;
	const delay_t	socket_linger_profile_t::TIMEOUT	= delay_t(delay_t::INFINITE_VAL);
#else	// debug value
	const bool	socket_linger_profile_t::ONOFF		= true;
	const delay_t	socket_linger_profile_t::TIMEOUT	= delay_t::from_sec(60);
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_linger_profile_t::socket_linger_profile_t()	throw()
{
	// set the default parameter
	onoff	(ONOFF);
	timeout	(TIMEOUT);
}

/** \brief Destructor
 */
socket_linger_profile_t::~socket_linger_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
socket_err_t	socket_linger_profile_t::check()	const throw()
{
	// return noerror
	return socket_err_t::OK;
}

NEOIP_NAMESPACE_END


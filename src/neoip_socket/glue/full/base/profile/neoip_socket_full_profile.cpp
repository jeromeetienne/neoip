/*! \file
    \brief Definition of the \ref socket_full_profile_t

*/


/* system include */
/* local include */
#include "neoip_socket_full_profile.hpp"
#include "neoip_socket_full.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref socket_full_profile_t constant
const bool	socket_full_profile_t::MTU_DISCOVERY	= false;
const size_t	socket_full_profile_t::RCVDATA_MAXLEN	= 16*1024;
const size_t	socket_full_profile_t::XMITBUF_MAXLEN	= socket_full_t::UNLIMITED_VAL;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_full_profile_t::socket_full_profile_t()	throw()
{
	mtu_discovery	(MTU_DISCOVERY);	// TODO this is not used currently
	rcvdata_maxlen	(RCVDATA_MAXLEN);
	xmitbuf_maxlen	(XMITBUF_MAXLEN);
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
socket_err_t	socket_full_profile_t::check()	const throw()
{
	// return no error
	return socket_err_t::OK;
}

NEOIP_NAMESPACE_END


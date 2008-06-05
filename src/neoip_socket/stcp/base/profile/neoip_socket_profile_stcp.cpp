/*! \file
    \brief definition of the \ref socket_profile_stcp_t
*/

/* system include */
/* local include */
#include "neoip_socket_profile_stcp.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_profile_vapi_t, socket_profile_stcp_t);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_profile_stcp_t::socket_profile_stcp_t()	throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return no error if the socket_profile_stcp_t is valid, .failed() otherwise
 */
socket_err_t	socket_profile_stcp_t::check()		const throw()
{
	// check the slay_profile_t
	slay_err_t	slay_err;
	slay_err	= slay().check();
	if( slay_err.failed() )	return socket_err_from_slay(slay_err);
	// return no error
	return socket_err_t::OK;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   access to domain specific api
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to access socket_profile_stcp_t of this socket_profile_stcp_t
 */
const socket_profile_stcp_t &socket_profile_stcp_t::from_socket(const socket_profile_t &socket_profile)	throw()
{
	// sanity check - the socket_domain_t MUST be stcp
	DBG_ASSERT( socket_profile.get_domain() == socket_domain_t::STCP );
	// return the socket_profile_stcp_api_t
	return dynamic_cast <const socket_profile_stcp_t &>(*socket_profile.profile_vapi());
}

/** \brief to access socket_profile_stcp_t of this socket_profile_stcp_t
 */
socket_profile_stcp_t &	socket_profile_stcp_t::from_socket(socket_profile_t &socket_profile)	throw()
{
	// sanity check - the socket_domain_t MUST be stcp
	DBG_ASSERT( socket_profile.get_domain() == socket_domain_t::STCP );
	// return the socket_profile_stcp_api_t
	return dynamic_cast <socket_profile_stcp_t &>(*socket_profile.profile_vapi());
}

NEOIP_NAMESPACE_END





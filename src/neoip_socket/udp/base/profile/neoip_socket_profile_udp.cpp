/*! \file
    \brief definition of the \ref socket_profile_udp_t
*/

/* system include */
/* local include */
#include "neoip_socket_profile_udp.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_profile_vapi_t, socket_profile_udp_t);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

socket_err_t	socket_profile_udp_t::check()		const throw()
{
	nlay_err_t	nlay_err;
	// check the inherited nlay_profile_t
	nlay_err	= nlay_profile_t::check();
	if( nlay_err.failed() )	return socket_err_from_nlay(nlay_err);
	// return no error
	return socket_err_t::OK;
}

/** \brief return the domain of this address
 */
socket_domain_t socket_profile_udp_t::get_domain()	const throw()
{
	return socket_domain_t::UDP;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   access to domain specific api
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to access socket_profile_udp_t of this socket_profile_udp_t
 */
const socket_profile_udp_t &socket_profile_udp_t::from_socket(const socket_profile_t &socket_profile)	throw()
{
	// sanity check - the socket_domain_t MUST be udp
	DBG_ASSERT( socket_profile.get_domain() == socket_domain_t::UDP );
	// return the socket_profile_udp_api_t
	return dynamic_cast <const socket_profile_udp_t &>(*socket_profile.profile_vapi());
}

/** \brief to access socket_profile_udp_t of this socket_profile_udp_t
 */
socket_profile_udp_t &	socket_profile_udp_t::from_socket(socket_profile_t &socket_profile)	throw()
{
	// sanity check - the socket_domain_t MUST be udp
	DBG_ASSERT( socket_profile.get_domain() == socket_domain_t::UDP );
	// return the socket_profile_udp_api_t
	return dynamic_cast <socket_profile_udp_t &>(*socket_profile.profile_vapi());
}

NEOIP_NAMESPACE_END





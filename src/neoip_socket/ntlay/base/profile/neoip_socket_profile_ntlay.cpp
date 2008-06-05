/*! \file
    \brief definition of the \ref socket_profile_ntlay_t
*/

/* system include */
/* local include */
#include "neoip_socket_profile_ntlay.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_profile_vapi_t, socket_profile_ntlay_t);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_profile_ntlay_t::socket_profile_ntlay_t()	throw()
{
	// zero some field
	ntudp_peer	( NULL );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief return the domain of this address
 */
socket_domain_t socket_profile_ntlay_t::get_domain()	const throw()
{
	return socket_domain_t::NTLAY;
}

/** \brief return no error if the socket_profile_ntlay_t is valid, .failed() otherwise
 */
socket_err_t	socket_profile_ntlay_t::check()		const throw()
{
	nlay_err_t	nlay_err;
	// check if the ntudp_peer_t is set
	if( ntudp_peer() == NULL )	return socket_err_t(socket_err_t::ERROR, "ntudp_peer_t is not set");	

	// check the inherited nlay_profile_t
	nlay_err	= nlay_profile_t::check();
	if( nlay_err.failed() )		return socket_err_from_nlay(nlay_err);
	// return no error
	return socket_err_t::OK;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   access to domain specific api
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to access socket_profile_ntlay_t of this socket_profile_ntlay_t
 */
const socket_profile_ntlay_t &socket_profile_ntlay_t::from_socket(const socket_profile_t &socket_profile)	throw()
{
	// sanity check - the socket_domain_t MUST be udp
	DBG_ASSERT( socket_profile.get_domain() == socket_domain_t::NTLAY );
	// return the socket_profile_udp_api_t
	return dynamic_cast <const socket_profile_ntlay_t &>(*socket_profile.profile_vapi());
}

/** \brief to access socket_profile_ntlay_t of this socket_profile_ntlay_t
 */
socket_profile_ntlay_t &	socket_profile_ntlay_t::from_socket(socket_profile_t &socket_profile)	throw()
{
	// sanity check - the socket_domain_t MUST be udp
	DBG_ASSERT( socket_profile.get_domain() == socket_domain_t::NTLAY );
	// return the socket_profile_udp_api_t
	return dynamic_cast <socket_profile_ntlay_t &>(*socket_profile.profile_vapi());
}


NEOIP_NAMESPACE_END





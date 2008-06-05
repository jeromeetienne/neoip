/*! \file
    \brief Declaration of the neoip_socket_udp_daddr
*/


#ifndef __NEOIP_SOCKET_PROFILE_UDP_HPP__
#define __NEOIP_SOCKET_PROFILE_UDP_HPP__
/* system include */
/* local include */
#include "neoip_socket_profile_vapi.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_namespace.hpp"

// include the types for the profile fields
#include "neoip_nlay_profile.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief socket_domain_t::UDP implementation of the socket_profile_vapi_t
 * 
 * - inherit of nlay_profile_t to ease the readability of the usage
 */
class socket_profile_udp_t : public socket_profile_vapi_t, public nlay_profile_t {
public:
	/*************** query function`***************************************/
	socket_err_t	check()					const throw();
	socket_domain_t	get_domain()				const throw();

	/*************** helper	***********************************************/
	static socket_profile_udp_t &		from_socket(socket_profile_t &socket_profile)		throw();
	static const socket_profile_udp_t &	from_socket(const socket_profile_t &socket_profile)	throw();

	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(socket_profile_vapi_t, socket_profile_udp_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PROFILE_UDP_HPP__ */




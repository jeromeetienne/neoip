/*! \file
    \brief Declaration of the neoip_socket_udp_daddr
*/


#ifndef __NEOIP_SOCKET_PROFILE_NTLAY_HPP__
#define __NEOIP_SOCKET_PROFILE_NTLAY_HPP__
/* system include */
/* local include */
#include "neoip_socket_profile_vapi.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_namespace.hpp"

// include the types for the profile fields
#include "neoip_nlay_profile.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ntudp_peer_t;

/** \brief socket_domain_t::NTLAY implementation of the socket_profile_vapi_t
 * 
 * - inherit of nlay_profile_t to ease the readability of the usage
 */
class socket_profile_ntlay_t : public socket_profile_vapi_t, public nlay_profile_t {
private:
	ntudp_peer_t *	ntudp_peer_val;	//!< the ntudp_peer_t on which to run	
public:
	/*************** ctor/dtor	***************************************/
	socket_profile_ntlay_t()	throw();
	/*************** query function`***************************************/
	socket_err_t	check()					const throw();
	socket_domain_t	get_domain()				const throw();

	/*************** helper	***********************************************/
	static socket_profile_ntlay_t &		from_socket(socket_profile_t &socket_profile)		throw();
	static const socket_profile_ntlay_t &	from_socket(const socket_profile_t &socket_profile)	throw();

	/*************** field access	***************************************/
	ntudp_peer_t *		ntudp_peer()			const throw() { return ntudp_peer_val;	}
	socket_profile_ntlay_t &ntudp_peer(ntudp_peer_t *value)	throw()
							{ this->ntudp_peer_val = value; return *this;	}
	

	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(socket_profile_vapi_t, socket_profile_ntlay_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PROFILE_NTLAY_HPP__ */




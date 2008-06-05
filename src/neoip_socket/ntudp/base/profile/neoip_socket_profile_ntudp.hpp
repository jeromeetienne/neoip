/*! \file
    \brief Declaration of the neoip_socket_ntudp_daddr
*/


#ifndef __NEOIP_SOCKET_PROFILE_NTUDP_HPP__
#define __NEOIP_SOCKET_PROFILE_NTUDP_HPP__
/* system include */
/* local include */
#include "neoip_socket_profile_vapi.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ntudp_peer_t;

/** \brief socket_domain_t::NTUDP implementation of the socket_profile_vapi_t
 */
class socket_profile_ntudp_t : public socket_profile_vapi_t {
private:
	ntudp_peer_t *	ntudp_peer_val;	//!< the ntudp_peer_t on which to run
public:
	/*************** ctor/dtor	***************************************/
	socket_profile_ntudp_t()	throw();

	/*************** query function	***************************************/
	socket_err_t	check()					const throw();
	socket_domain_t	get_domain()				const throw();
	
	
	/*************** helper	***********************************************/
	static socket_profile_ntudp_t &		from_socket(socket_profile_t &socket_profile)		throw();
	static const socket_profile_ntudp_t &	from_socket(const socket_profile_t &socket_profile)	throw();

	/*************** field access	***************************************/
	ntudp_peer_t *		ntudp_peer()			const throw() { return ntudp_peer_val;	}
	socket_profile_ntudp_t &ntudp_peer(ntudp_peer_t *value)	throw()
							{ this->ntudp_peer_val = value; return *this;	}
	

	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(socket_profile_vapi_t, socket_profile_ntudp_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PROFILE_NTUDP_HPP__ */




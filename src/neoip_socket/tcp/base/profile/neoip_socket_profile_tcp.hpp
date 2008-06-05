/*! \file
    \brief Declaration of the neoip_socket_tcp_daddr
*/


#ifndef __NEOIP_SOCKET_PROFILE_TCP_HPP__
#define __NEOIP_SOCKET_PROFILE_TCP_HPP__
/* system include */
/* local include */
#include "neoip_socket_profile_vapi.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief socket_domain_t::TCP implementation of the socket_profile_vapi_t
 */
class socket_profile_tcp_t : public socket_profile_vapi_t {
private:
public:
	/*************** ctor/dtor	***************************************/
	socket_profile_tcp_t()	throw();

	/*************** query function	***************************************/
	socket_err_t	check()					const throw();
	socket_domain_t	get_domain()				const throw();
	
	/*************** helper	***********************************************/
	static socket_profile_tcp_t &		from_socket(socket_profile_t &socket_profile)		throw();
	static const socket_profile_tcp_t &	from_socket(const socket_profile_t &socket_profile)	throw();

	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(socket_profile_vapi_t, socket_profile_tcp_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PROFILE_TCP_HPP__ */




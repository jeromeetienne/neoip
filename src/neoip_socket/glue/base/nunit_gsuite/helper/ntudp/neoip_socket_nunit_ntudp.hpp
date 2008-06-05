/*! \file
    \brief Declaration of the neoip_socket_udp_daddr
*/


#ifndef __NEOIP_SOCKET_NUNIT_NTUDP_HPP__
#define __NEOIP_SOCKET_NUNIT_NTUDP_HPP__
/* system include */
/* local include */
#include "neoip_socket_nunit_domain_vapi.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ntudp_peer_t;

/** \brief helper specific to socket_domain_t::NTUDP for the socket unit
 */
class socket_nunit_ntudp_t : public socket_nunit_domain_vapi_t {
private:
	socket_addr_t		server_addr;
	socket_domain_t		socket_domain;
	socket_type_t		socket_type;
	socket_profile_t	socket_profile;
	
	ntudp_peer_t *		ntudp_peer;
public:
	/*************** ctor/dtor	***************************************/
	socket_nunit_ntudp_t()		throw();
	~socket_nunit_ntudp_t()		throw();
	
	/*************** query function	***************************************/
	socket_resp_arg_t	get_resp_arg()	const throw();
	socket_itor_arg_t	get_itor_arg()	const throw();
	const socket_domain_t &	get_domain()	const throw() { return socket_domain;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_NUNIT_NTUDP_HPP__ */




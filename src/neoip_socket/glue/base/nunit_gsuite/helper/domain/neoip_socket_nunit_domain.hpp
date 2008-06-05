/*! \file
    \brief Declaration of the neoip_socket_udp_daddr
*/


#ifndef __NEOIP_SOCKET_NUNIT_DOMAIN_HPP__
#define __NEOIP_SOCKET_NUNIT_DOMAIN_HPP__
/* system include */
/* local include */
#include "neoip_socket_nunit_domain_vapi.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief wrapper aroung the nunit helper specific of each domain
 */
class socket_nunit_domain_t  {
private:
	socket_nunit_domain_vapi_t *	domain_vapi;
public:
	/*************** ctor/dtor	***************************************/
	socket_nunit_domain_t(const socket_domain_t &socket_domain)	throw();
	~socket_nunit_domain_t()					throw();
	
	/*************** query function	***************************************/
	socket_resp_arg_t	get_resp_arg()	const throw()	{ return domain_vapi->get_resp_arg();	}
	socket_itor_arg_t	get_itor_arg()	const throw()	{ return domain_vapi->get_itor_arg();	}
	const socket_domain_t &	get_domain()	const throw()	{ return domain_vapi->get_domain();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_NUNIT_DOMAIN_HPP__ */




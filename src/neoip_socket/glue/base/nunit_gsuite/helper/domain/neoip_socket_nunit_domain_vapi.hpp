/*! \file
    \brief Header of the \ref socket_nunit_domain_vapi_t
*/


#ifndef __NEOIP_SOCKET_NUNIT_DOMAIN_VAPI_HPP__ 
#define __NEOIP_SOCKET_NUNIT_DOMAIN_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_socket_resp_arg.hpp"
#include "neoip_socket_itor_arg.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Definition of the virtual API for the socket_nunit_domain
 */
class socket_nunit_domain_vapi_t {
public:
	/*************** query function	***************************************/
	virtual socket_resp_arg_t	get_resp_arg()	const throw() = 0;
	virtual	socket_itor_arg_t	get_itor_arg()	const throw() = 0;
	virtual	const socket_domain_t &	get_domain()	const throw() = 0;
	
	//! virtual destructor
	virtual ~socket_nunit_domain_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_NUNIT_DOMAIN_VAPI_HPP__  */




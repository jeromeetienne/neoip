/*! \file
    \brief definition of the \ref socket_nunit_tcp_t
*/

/* system include */
/* local include */
#include "neoip_socket_nunit_tcp.hpp"
#include "neoip_log.hpp"

// include to access the domain specific profile
#include "neoip_socket_profile_tcp.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_nunit_tcp_t::socket_nunit_tcp_t()	throw()
{
	server_addr	= "tcp://127.0.0.1:4000";
	socket_domain	= socket_domain_t::TCP;
	socket_type	= socket_type_t::STREAM;
	socket_profile	= socket_profile_t(socket_domain);
}

/** \brief destructor
 */
socket_nunit_tcp_t::~socket_nunit_tcp_t()	throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	
/** \brief Return a socket_resp_arg_t for this domain
 */
socket_resp_arg_t	socket_nunit_tcp_t::get_resp_arg()	const throw()
{
	socket_resp_arg_t	resp_arg;
	// build the resp_arg
	resp_arg	= socket_resp_arg_t().profile(socket_profile).domain(socket_domain)
							.type(socket_type).listen_addr(server_addr);	
	// return the resp_arg
	return resp_arg;
}

/** \brief Return a socket_itor_arg_t for this domain
 */
socket_itor_arg_t	socket_nunit_tcp_t::get_itor_arg()	const throw()
{
	socket_itor_arg_t	itor_arg;
	// build the itor_arg
	itor_arg	= socket_itor_arg_t().profile(socket_profile).domain(socket_domain)
							.type(socket_type).remote_addr(server_addr);	
	// return the itor_arg
	return itor_arg;
}

NEOIP_NAMESPACE_END





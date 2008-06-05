/*! \file
    \brief Definition of static helper functions for the tcp layer

*/

/* system include */
/* local include */
#include "neoip_socket_helper_tcp.hpp"
#include "neoip_socket_peerid_tcp.hpp"
#include "neoip_socket_portid_tcp.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_tcp_profile.hpp"
#include "neoip_log.hpp"

// list of include for the insertion in factory_plant_t
#include "neoip_socket_profile_tcp.hpp"
#include "neoip_socket_peerid_tcp.hpp"
#include "neoip_socket_portid_tcp.hpp"
#include "neoip_socket_resp_tcp.hpp"
#include "neoip_socket_itor_tcp.hpp"
#include "neoip_socket_full_tcp.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref socket_helper_tcp_t constant
const socket_domain_t	socket_helper_tcp_t::DOMAIN_VAR	= strtype_socket_domain_t::TCP;
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Convert a socket_profile_t into a tcp_profile_t
 * 
 */
tcp_profile_t	socket_helper_tcp_t::socket_to_tcp_profile(const socket_profile_t &socket_profile) throw()
{
	tcp_profile_t	tcp_profile;
	// sanity check - the profile MUST valid
	DBG_ASSERT( socket_profile.check().succeed() );
	// convert the socket_profile_t
	tcp_profile.itor().timeout_delay	( socket_profile.itor_timeout()		);
	tcp_profile.full().rcvdata_maxlen	( socket_profile.full().rcvdata_maxlen());
	tcp_profile.full().xmitbuf_maxlen	( socket_profile.full().xmitbuf_maxlen());
	tcp_profile.full().xmit_limit_arg	( socket_profile.full().xmit_limit_arg());
	tcp_profile.full().recv_limit_arg	( socket_profile.full().recv_limit_arg());
	// return the just built tcp_profile_t
	return tcp_profile;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Helper function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Static function which return the ipport_addr_t from a socket_addr_t
 */
ipport_addr_t	socket_helper_tcp_t::ipport_addr(const socket_addr_t &socket_addr)	throw()
{
	// sanity check - socket_addr MUST be of the proper socket_domain_t
	DBG_ASSERT( socket_addr.get_domain() == socket_domain_t::TCP );
	DBG_ASSERT( typeid(*socket_addr.get_peerid_vapi()) == typeid(socket_peerid_tcp_t));
	DBG_ASSERT( typeid(*socket_addr.get_portid_vapi()) == typeid(socket_portid_tcp_t));
	// get the ip_addr_t from the socket_peerid_t
	ip_addr_t	ip_addr	= (dynamic_cast <const socket_peerid_tcp_t &>(*socket_addr.get_peerid_vapi())).ipaddr;
	// get the port from the socket_portid_t
	uint16_t	port	= (dynamic_cast <const socket_portid_tcp_t &>(*socket_addr.get_portid_vapi())).port;
	// return the ipport_addr_t
	return ipport_addr_t(ip_addr, port);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			factory_domain_insert
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Populate the socket_*_t factory with all the socket_domain_t::TCP class
 */
void	socket_helper_tcp_t::factory_domain_insert()				throw()
{
	// insert all the factory_product_t for strtype_socket_domain_t::TCP
	FACTORY_PRODUCT_INSERT(socket_profile_factory, socket_domain_t, socket_profile_vapi_t
				, strtype_socket_domain_t::TCP, socket_profile_tcp_t);
	FACTORY_PRODUCT_INSERT(socket_peerid_factory, socket_domain_t, socket_peerid_vapi_t
				, strtype_socket_domain_t::TCP, socket_peerid_tcp_t);
	FACTORY_PRODUCT_INSERT(socket_portid_factory, socket_domain_t, socket_portid_vapi_t
				, strtype_socket_domain_t::TCP, socket_portid_tcp_t);
	FACTORY_PRODUCT_INSERT(socket_resp_factory, socket_domain_t, socket_resp_vapi_t
				, strtype_socket_domain_t::TCP, socket_resp_tcp_t);
	FACTORY_PRODUCT_INSERT(socket_itor_factory, socket_domain_t, socket_itor_vapi_t
				, strtype_socket_domain_t::TCP, socket_itor_tcp_t);
	FACTORY_PRODUCT_INSERT(socket_full_factory, socket_domain_t, socket_full_vapi_t
				, strtype_socket_domain_t::TCP, socket_full_tcp_t);
}


NEOIP_NAMESPACE_END;







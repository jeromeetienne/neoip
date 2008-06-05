/*! \file
    \brief Definition of static helper functions for the udp layer

*/

/* system include */
/* local include */
#include "neoip_socket_helper_udp.hpp"
#include "neoip_socket_peerid_udp.hpp"
#include "neoip_socket_portid_udp.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_log.hpp"

// list of include for the insertion in factory_plant_t
#include "neoip_socket_profile_udp.hpp"
#include "neoip_socket_peerid_udp.hpp"
#include "neoip_socket_portid_udp.hpp"
#include "neoip_socket_resp_udp.hpp"
#include "neoip_socket_itor_udp.hpp"
#include "neoip_socket_full_udp.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref socket_helper_udp_t constant
const socket_domain_t	socket_helper_udp_t::DOMAIN_VAR	= strtype_socket_domain_t::UDP;
// end of constants definition

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Helper function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Static function which return the ipport_addr_t from a socket_addr_t
 */
ipport_addr_t	socket_helper_udp_t::ipport_addr(const socket_addr_t &socket_addr)	throw()
{
	// sanity check - socket_addr MUST be of the proper socket_domain_t
	DBG_ASSERT( socket_addr.get_domain() == socket_domain_t::UDP );
	DBG_ASSERT( typeid(*socket_addr.get_peerid_vapi()) == typeid(socket_peerid_udp_t));
	DBG_ASSERT( typeid(*socket_addr.get_portid_vapi()) == typeid(socket_portid_udp_t));
	// get the ip_addr_t from the socket_peerid_t
	ip_addr_t	ip_addr	= (dynamic_cast <const socket_peerid_udp_t &>(*socket_addr.get_peerid_vapi())).ipaddr;
	// get the port from the socket_portid_t
	uint16_t	port	= (dynamic_cast <const socket_portid_udp_t &>(*socket_addr.get_portid_vapi())).port;
	// return the ipport_addr_t
	return ipport_addr_t(ip_addr, port);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			factory_domain_insert
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Populate the socket_*_t factory with all the socket_domain_t::UDP class
 */
void	socket_helper_udp_t::factory_domain_insert()				throw()
{
	// insert all the factory_product_t for strtype_socket_domain_t::UDP
	FACTORY_PRODUCT_INSERT(socket_profile_factory, socket_domain_t, socket_profile_vapi_t
				, strtype_socket_domain_t::UDP, socket_profile_udp_t);
	FACTORY_PRODUCT_INSERT(socket_peerid_factory, socket_domain_t, socket_peerid_vapi_t
				, strtype_socket_domain_t::UDP, socket_peerid_udp_t);
	FACTORY_PRODUCT_INSERT(socket_portid_factory, socket_domain_t, socket_portid_vapi_t
				, strtype_socket_domain_t::UDP, socket_portid_udp_t);
	FACTORY_PRODUCT_INSERT(socket_resp_factory, socket_domain_t, socket_resp_vapi_t
				, strtype_socket_domain_t::UDP, socket_resp_udp_t);
	FACTORY_PRODUCT_INSERT(socket_itor_factory, socket_domain_t, socket_itor_vapi_t
				, strtype_socket_domain_t::UDP, socket_itor_udp_t);
	FACTORY_PRODUCT_INSERT(socket_full_factory, socket_domain_t, socket_full_vapi_t
				, strtype_socket_domain_t::UDP, socket_full_udp_t);
}

NEOIP_NAMESPACE_END;







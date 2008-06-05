/*! \file
    \brief Definition of static helper functions for the ntudp layer

*/

/* system include */
/* local include */
#include "neoip_socket_helper_ntudp.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_log.hpp"

// list of include for the insertion in factory_plant_t
#include "neoip_socket_profile_ntudp.hpp"
#include "neoip_socket_peerid_ntudp.hpp"
#include "neoip_socket_portid_ntudp.hpp"
#include "neoip_socket_resp_ntudp.hpp"
#include "neoip_socket_itor_ntudp.hpp"
#include "neoip_socket_full_ntudp.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref socket_helper_ntudp_t constant
const socket_domain_t	socket_helper_ntudp_t::DOMAIN_VAR	= strtype_socket_domain_t::NTUDP;
// end of constants definition


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			factory_domain_insert
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Populate the socket_*_t factory with all the socket_domain_t::NTUDP class
 */
void	socket_helper_ntudp_t::factory_domain_insert()				throw()
{
	// insert all the factory_product_t for strtype_socket_domain_t::NTUDP
	FACTORY_PRODUCT_INSERT(socket_profile_factory, socket_domain_t, socket_profile_vapi_t
				, strtype_socket_domain_t::NTUDP, socket_profile_ntudp_t);
	FACTORY_PRODUCT_INSERT(socket_peerid_factory, socket_domain_t, socket_peerid_vapi_t
				, strtype_socket_domain_t::NTUDP, socket_peerid_ntudp_t);
	FACTORY_PRODUCT_INSERT(socket_portid_factory, socket_domain_t, socket_portid_vapi_t
				, strtype_socket_domain_t::NTUDP, socket_portid_ntudp_t);
	FACTORY_PRODUCT_INSERT(socket_resp_factory, socket_domain_t, socket_resp_vapi_t
				, strtype_socket_domain_t::NTUDP, socket_resp_ntudp_t);
	FACTORY_PRODUCT_INSERT(socket_itor_factory, socket_domain_t, socket_itor_vapi_t
				, strtype_socket_domain_t::NTUDP, socket_itor_ntudp_t);
	FACTORY_PRODUCT_INSERT(socket_full_factory, socket_domain_t, socket_full_vapi_t
				, strtype_socket_domain_t::NTUDP, socket_full_ntudp_t);
}

NEOIP_NAMESPACE_END;







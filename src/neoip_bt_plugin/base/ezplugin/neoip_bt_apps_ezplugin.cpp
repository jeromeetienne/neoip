/*! \file
    \brief Definition of the \ref bt_apps_ezplugin_t class
    
*/

/* system include */
/* local include */
#include "neoip_bt_apps_ezplugin.hpp"
#include "neoip_log.hpp"

// list of include for the factory_product_t insertion of the socket_*_t layer
#include "neoip_socket_helper_udp.hpp"
#include "neoip_socket_helper_ntudp.hpp"
#include "neoip_socket_helper_ntlay.hpp"
#include "neoip_socket_helper_tcp.hpp"
#include "neoip_socket_helper_stcp.hpp"

// list of include for the factory_product_t insertion of the slay_*_t layer
#include "neoip_slay_helper_btjamrc4.hpp"
#include "neoip_slay_helper_btjamstd.hpp"
#include "neoip_slay_helper_tls.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			misc
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert all the plugins available
 * 
 * - WARNING: this function is mostly likely to be called before initializing
 *   lib_session_t 
 */
void	bt_apps_ezplugin_t::insert_all_needed()	throw()
{
	// insert all the socket_*_t factory_product_t
	socket_helper_tcp_t	::factory_domain_insert();
	socket_helper_stcp_t	::factory_domain_insert();
	// insert all the slay_*_t factory_product_t
	slay_helper_btjamrc4_t	::factory_domain_insert();
}




NEOIP_NAMESPACE_END



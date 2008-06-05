/*! \file
    \brief Definition of the unit test for the \ref socket_client_t

*/

/* system include */
/* local include */
#include "neoip_socket_nunit_domain.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

// include for all the subdomain
#include "neoip_socket_nunit_udp.hpp"
#include "neoip_socket_nunit_ntudp.hpp"
#include "neoip_socket_nunit_ntlay.hpp"
#include "neoip_socket_nunit_tcp.hpp"
#include "neoip_socket_nunit_stcp.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_nunit_domain_t::socket_nunit_domain_t(const socket_domain_t &socket_domain)	throw()
{
	// if the nunit_idstr is "nlay_udp"
	if( socket_domain == socket_domain_t::UDP ){
		domain_vapi	= nipmem_new socket_nunit_udp_t();
		return;
	}else if( socket_domain == socket_domain_t::NTUDP ){
		domain_vapi	= nipmem_new socket_nunit_ntudp_t();
		return;
	}else if( socket_domain == socket_domain_t::NTLAY ){
		domain_vapi	= nipmem_new socket_nunit_ntlay_t();
		return;
	}else if( socket_domain == socket_domain_t::TCP ){
		domain_vapi	= nipmem_new socket_nunit_tcp_t();
		return;
	}else if( socket_domain == socket_domain_t::STCP ){
		domain_vapi	= nipmem_new socket_nunit_stcp_t();
		return;
	}
	// NOTE: this point should never be reached
	DBG_ASSERT( 0 );
}

/** \brief Destructor
 */
socket_nunit_domain_t::~socket_nunit_domain_t()					throw()
{
	// delete the domain_vapi 
	// - NOTE: here the dynamic_cast is used to get the same pointer for the nipmem_new and 
	//   nipmem_delete. Here there are different as delete is done on a base class and new 
	//   is done on the derived one.
	// - thus the debug layer which keep track of the allocated/freed memory dont get confused
	if( get_domain() == socket_domain_t::UDP ){
		nipmem_delete	dynamic_cast<socket_nunit_udp_t *>(domain_vapi);
		return;
	}else if( get_domain() == socket_domain_t::NTUDP ){
		nipmem_delete	dynamic_cast<socket_nunit_ntudp_t *>(domain_vapi);
		return;
	}else if( get_domain() == socket_domain_t::NTLAY ){
		nipmem_delete	dynamic_cast<socket_nunit_ntlay_t *>(domain_vapi);
		return;
	}else if( get_domain() == socket_domain_t::TCP ){
		nipmem_delete	dynamic_cast<socket_nunit_tcp_t *>(domain_vapi);
		return;
	}else if( get_domain() == socket_domain_t::STCP ){
		nipmem_delete	dynamic_cast<socket_nunit_stcp_t *>(domain_vapi);
		return;
	}
	// NOTE: this point should never be reached
	DBG_ASSERT( 0 );
}


NEOIP_NAMESPACE_END


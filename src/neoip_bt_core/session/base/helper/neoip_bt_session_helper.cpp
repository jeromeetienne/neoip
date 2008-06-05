/*! \file
    \brief Definition of static helper functions for the tls layer

*/

/* system include */
/* local include */
#include "neoip_bt_session_helper.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_peersrc_peer.hpp"
#include "neoip_socket_itor_arg.hpp"
#include "neoip_socket_resp_arg.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_log.hpp"

// all the include for the socket_profile_t and sub-domain
#include "neoip_bt_jamrc4_profile.hpp"
#include "neoip_slay_profile_btjamrc4.hpp"
#include "neoip_socket_helper_tcp.hpp"
#include "neoip_socket_profile_stcp.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a socket_itor_arg_t for this bt_swarm_peersrc_t
 * 
 * - it regroups all socket_itor_arg_t building
 */
socket_itor_arg_t bt_session_helper_t::build_socket_itor_arg(const bt_peersrc_peer_t &peersrc_peer
							, bt_swarm_t *bt_swarm)	throw()
{
	socket_itor_arg_t	itor_arg;
	// log to debug
	KLOG_ERR("enter peersrc_peer=" << peersrc_peer);
#if 0	// TODO NOTE: need to be removed ? old stuff on top of socket_domain_t::TCP
	socket_profile_t	socket_profile(socket_domain_t::TCP);
	// build the socket_itor_arg_t for the bt_swarm_itor_t
	itor_arg	= socket_itor_arg_t().profile(socket_profile)
				.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
				.remote_addr(peersrc_peer.dest_addr());
#else
	// set socket_profile on socket_domain_t::STCP
	socket_profile_t socket_profile	= build_socket_profile_stcp(bt_swarm->get_session(), bt_swarm);
	// build the dest_addr socket_addr_t
	// - use some trick because the peersrc_peer.dest_addr() is in fact
	//   using socket_domain_t::TCP all over the codes
	//   - TODO this is dirty to get this hardcoded in other part of the code. fix it
	//ipport_addr_t	dest_ipport	= socket_helper_tcp_t::ipport_addr(peersrc_peer.dest_addr());
	//socket_addr_t	dest_addr	= std::string("stcp://") + dest_ipport.to_string();
	socket_addr_t	dest_addr	= peersrc_peer.dest_addr();
	// build the socket_itor_arg_t for the bt_swarm_itor_t
	itor_arg	= socket_itor_arg_t().profile(socket_profile)
				.domain(socket_domain_t::STCP).type(socket_type_t::STREAM)
				.remote_addr(dest_addr);
#endif
	// return the just-built socket_itor_arg_t
	return itor_arg;
}

/** \brief Return a socket_resp_arg_t for this listen_ipport
 * 
 * - it regroups all socket_resp_arg_t building 
 */
socket_resp_arg_t bt_session_helper_t::build_socket_resp_arg(const ipport_addr_t &listen_ipport
						, bt_session_t *bt_session)	throw()
{
	socket_resp_arg_t resp_arg;
	// log to debug
	KLOG_ERR("enter listen_ipport=" << listen_ipport);
#if 0	// TODO NOTE: need to be removed ? old stuff on top of socket_domain_t::TCP
	// build the socket_profile_t for the socket_resp_t
	socket_profile_t	socket_profile(socket_domain_t::TCP);
	// build the resp_arg
	socket_addr_t	listen_addr	= std::string("tcp://") + listen_ipport.to_string();
	resp_arg	= socket_resp_arg_t().profile(socket_profile)
				.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
				.listen_addr(listen_addr);
#else
	// set socket_profile on socket_domain_t::STCP
	socket_profile_t socket_profile	= build_socket_profile_stcp(bt_session, NULL);
	// build the listen_addr
	socket_addr_t	listen_addr	= std::string("stcp://") + listen_ipport.to_string();
	resp_arg	= socket_resp_arg_t().profile(socket_profile)
				.domain(socket_domain_t::STCP).type(socket_type_t::STREAM)
				.listen_addr(listen_addr);
#endif
	// return the just-built socket_resp_arg_t
	return resp_arg;
}


/** \brief Build a socket_profile_t for this bt_session_t/bt_swarm_t
 * 
 * - NOTE: bt_swarm MAY be null, if called for socket_resp_arg_t. 
 *   - in this case, jamrc4_profile.itor_type_default() is not set but it is ok
 *     as it is not used by bt_jamrc4_resp_t anyway
 */
socket_profile_t bt_session_helper_t::build_socket_profile_stcp(bt_session_t *bt_session
						, bt_swarm_t *bt_swarm)		throw()
{
	// build the bt_jamrc4_profile_t
	bt_jamrc4_profile_t	jamrc4_profile;
	jamrc4_profile.resp_type_accept_arr	( bt_session->profile().resp_jamrc4_type_arr());
	if( bt_swarm )	jamrc4_profile.itor_type_default( bt_swarm->profile().itor_jamrc4_type());
	// set bt_jamrc4_profile_t into slay_profile_t
	slay_profile_t		slay_profile(slay_domain_t::BTJAMRC4);
	slay_profile_btjamrc4_t&slay_profile_dom= slay_profile_btjamrc4_t::from_slay(slay_profile);
	slay_profile_dom.bt_jamrc4	(jamrc4_profile);
	// set slay_profile_t into socket_profile_t
	socket_profile_t	socket_profile(socket_domain_t::STCP);
	socket_profile_stcp_t &	socket_profile_dom	= socket_profile_stcp_t::from_socket(socket_profile);
	socket_profile_dom.slay		(slay_profile);
	// return the just-built socket_profile_t
	return socket_profile;
}

NEOIP_NAMESPACE_END;







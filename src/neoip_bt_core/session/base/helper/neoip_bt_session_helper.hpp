/*! \file
    \brief Declaration of static helper functions for bt_session_t
    
*/


#ifndef __NEOIP_BT_SESSION_HELPER_HPP__ 
#define __NEOIP_BT_SESSION_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_session_t;
class	bt_swarm_t;
class	bt_peersrc_peer_t;
class	socket_profile_t;
class	socket_itor_arg_t;
class	socket_resp_arg_t;
class	ipport_addr_t;

/** \brief static helpers for bt_session_t
 */
class bt_session_helper_t {
private:
	static socket_profile_t	build_socket_profile_stcp(bt_session_t *bt_session
						, bt_swarm_t *bt_swarm)		throw();
public:
	static socket_itor_arg_t build_socket_itor_arg(const bt_peersrc_peer_t &peersrc_peer
						, bt_swarm_t *bt_swarm)		throw();
	static socket_resp_arg_t build_socket_resp_arg(const ipport_addr_t &listen_ipport
						, bt_session_t *bt_session)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SESSION_HELPER_HPP__  */











/*! \file
    \brief Declaration of the ntudp_relpeer_tunnel_t
    
*/


#ifndef __NEOIP_NTUDP_RELPEER_TUNNEL_HPP__ 
#define __NEOIP_NTUDP_RELPEER_TUNNEL_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_relpeer_tunnel_cb.hpp"
#include "neoip_ntudp_relpeer_tunnel_wikidbg.hpp"
#include "neoip_ntudp_relpeer_tunnel_profile.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_udp_client_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_timeout.hpp"
#include "neoip_pkt.hpp"
#include "neoip_serial.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class ntudp_peer_t;

/** \brief Establish and maintain a tunnel toward a \ref ntudp_pserver_t
 */
class ntudp_relpeer_tunnel_t : NEOIP_COPY_CTOR_DENY, private udp_client_cb_t, private timeout_cb_t
		, private wikidbg_obj_t<ntudp_relpeer_tunnel_t, ntudp_relpeer_tunnel_wikidbg_init>
		{
private:
	/*************** internal data	***************************************/
	ntudp_nonce_t			client_nonce;	//!< the client_nonce identify this tunnel
	ntudp_peer_t *			ntudp_peer;	//!< pointer on the ntudp_peer_t for which this 
							//!< tunnel is established	
	bool				itor_inprogress;//!< true if the tunnel_t is in itor, false if it is in
							//!< full phase.
	ntudp_relpeer_tunnel_profile_t	profile;	//!< the profile attached to this object

	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;	//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;	//!< to periodically send packet over the udp_client_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** packet building	*******************************/
	pkt_t 	build_pkt_register_request()					const throw();	
	pkt_t	build_pkt_cli2srv(const slot_id_t &extcnx_slotid,const datum_t &datum_tofwd)const throw();
	
	/*************** udp_client	***************************************/
	udp_client_t *	udp_client;		//!< the udp_client_t on which the protocol occurs
	bool 		neoip_udp_client_event_cb(void *cb_userptr, udp_client_t &cb_udp_client
							, const udp_event_t &udp_event)	throw();

	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)			throw();
	bool		recv_register_reply(pkt_t &pkt)			throw(serial_except_t);
	bool		recv_pkt_srv2cli(pkt_t &pkt)			throw(serial_except_t);

	/*************** Callback	***************************************/
	ntudp_relpeer_tunnel_cb_t *	callback;	//!< the callback to notify result
	void *				userptr;	//!< the userptr associated with the callback
	bool				notify_callback(const bool succeed)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_relpeer_tunnel_t()		throw();
	~ntudp_relpeer_tunnel_t()		throw();
	
	/*************** Setup function	***************************************/
	ntudp_relpeer_tunnel_t &set_profile(const ntudp_relpeer_tunnel_profile_t &profile)	throw();	
	ntudp_err_t	start(const ipport_addr_t &pserver_addr, ntudp_peer_t *ntudp_peer
					, ntudp_relpeer_tunnel_cb_t *callback, void * userptr)	throw();
	
	/*************** Query function	***************************************/
	const ipport_addr_t &	get_pserver_addr()	const throw();
	bool			is_established()	const throw()	{ return !itor_inprogress;	}

	/*************** List of friend classes	*******************************/
	friend class ntudp_relpeer_tunnel_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RELPEER_TUNNEL_HPP__  */




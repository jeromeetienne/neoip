/*! \file
    \brief Declaration of the ntudp_pserver_t
    
*/


#ifndef __NEOIP_NTUDP_PSERVER_TUNNEL_HPP__ 
#define __NEOIP_NTUDP_PSERVER_TUNNEL_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_pserver.hpp"
#include "neoip_ntudp_pserver_tunnel_wikidbg.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_peerid.hpp"
#include "neoip_timeout.hpp"
#include "neoip_pkt.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief handle a given connection received by the ntudp_pserver_t from
 *         a ntudp_relpeer_t
 */
class ntudp_pserver_tunnel_t : NEOIP_COPY_CTOR_DENY, private udp_full_cb_t, private timeout_cb_t
		, private wikidbg_obj_t<ntudp_pserver_tunnel_t, ntudp_pserver_tunnel_wikidbg_init>
		{
private:
	ntudp_pserver_t *	ntudp_pserver;	//!< backpointer to the ntudp_pserver_t
	ntudp_peerid_t		client_peerid;	//!< the registered peerid of the client
	ntudp_nonce_t		client_nonce;	//!< the nonce sent by the client for this tunnel

	/*************** udp_full_t	***************************************/
	udp_full_t *	udp_full;	//!< the udp_full_t on which this connection occurs
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw();
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		recv_register_request(pkt_t &pkt)			throw(serial_except_t);
	bool		recv_pkt_cli2srv(pkt_t &pkt)				throw(serial_except_t);

	/*************** idle_timout	***************************************/
	timeout_t	idle_timeout;		//!< to timeout tunnel_t when no packet are received for
						//!< IDLE_TIMEOUT_DELAY.
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** packet build	***************************************/
	pkt_t	build_register_reply(const ntudp_nonce_t &echoed_nonce, bool accepted_f)	const throw();
	pkt_t	build_pkt_srv2cli(const slot_id_t &extcnx_slotid,const datum_t &datum_tofwd)	const throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_pserver_tunnel_t(ntudp_pserver_t *ntudp_pserver, udp_full_t *udp_full)	throw();
	~ntudp_pserver_tunnel_t()							throw();
	
	/*************** query function	***************************************/
	const ntudp_peerid_t &	registered_peerid()	const throw() { return client_peerid;		}
	bool			is_established()	const throw() { return !client_peerid.is_null();}

	/*************** action function	*******************************/
	void	send_pkt_srv2cli(const slot_id_t &extcnx_slotid, const datum_t &datum_tofwd)	throw();

	/*************** List of friend classes	*******************************/
	friend class ntudp_pserver_tunnel_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PSERVER_TUNNEL_HPP__  */




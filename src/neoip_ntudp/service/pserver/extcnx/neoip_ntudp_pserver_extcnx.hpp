/*! \file
    \brief Declaration of the ntudp_pserver_t
    
*/


#ifndef __NEOIP_NTUDP_PSERVER_EXTCNX_HPP__ 
#define __NEOIP_NTUDP_PSERVER_EXTCNX_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_pserver.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_pkt.hpp"
#include "neoip_timeout.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief handle a given connection received by the ntudp_pserver_t from
 *         an 'external peer'
 */
class ntudp_pserver_extcnx_t : NEOIP_COPY_CTOR_DENY, private udp_full_cb_t, private timeout_cb_t {
private:
	ntudp_pserver_t *	ntudp_pserver;	//!< backpointer to the ntudp_pserver_t
	ntudp_nonce_t		client_nonce;	//!< the ntudp_nonce_t from the external peer
	slot_id_t		slot_id;	//!< the pseudo unique slot_id_t for this extcnx

	/*************** udp_full_t	***************************************/
	udp_full_t *	udp_full;	//!< the udp_full_t on which this connection occurs
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw();
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)			throw();
	bool		recv_pkt_ext2srv(pkt_t &pkt)			throw(serial_except_t);

	/*************** idle_timout	***************************************/
	timeout_t	timeout;	//!< to timeout extcnx_t when no packet are received back
					//!< from the ntudp_pserver_tunnel_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_pserver_extcnx_t(ntudp_pserver_t *ntudp_pserver, udp_full_t *udp_full)	throw();
	~ntudp_pserver_extcnx_t()							throw();
	
	/*************** Query function	***************************************/
	slot_id_t	get_slotid()	const throw()	{ return slot_id;	}
	
	/*************** Utility function	*******************************/
	void		notify_recved_reply(pkt_t &pkt_reply)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PSERVER_EXTCNX_HPP__  */




/*! \file
    \brief Declaration of the udp_vresp_t
    
*/


#ifndef __NEOIP_UDP_VRESP_CNX_HPP__ 
#define __NEOIP_UDP_VRESP_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_udp_vresp.hpp"
#include "neoip_pkt.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief handle a given connection received by the udp_vresp_t
 * 
 * - it aims to wait for the first packet, read the pkttype, notify 
 *   the udp_full_t immediatly to the proper callback and do a zerotimer 
 *   before notifying the packet.
 *   - after that, the udp_full_t is owned by the callback
 */
class udp_vresp_cnx_t : private udp_full_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	udp_vresp_t *	udp_vresp;	//!< backpointer to the udp_vresp_t
	
	/*************** udp_full_t	***************************************/
	udp_full_t *	udp_full;	//!< the udp_full_t 
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
					, const udp_event_t &udp_event)		throw();
	void		handle_recved_data(pkt_t &pkt)	throw();
public:
	/*************** ctor/dtor	***************************************/
	udp_vresp_cnx_t(udp_vresp_t *udp_vresp, udp_full_t *udp_full)	throw();
	~udp_vresp_cnx_t()						throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_VRESP_CNX_HPP__  */




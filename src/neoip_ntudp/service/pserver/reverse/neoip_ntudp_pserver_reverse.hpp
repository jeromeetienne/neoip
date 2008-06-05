/*! \file
    \brief Declaration of the ntudp_pserver_t
    
*/


#ifndef __NEOIP_NTUDP_PSERVER_REVERSE_HPP__ 
#define __NEOIP_NTUDP_PSERVER_REVERSE_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_pserver.hpp"
#include "neoip_timeout.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	pkt_t;

/** \brief handle a given connection received by the ntudp_pserver_t for a REVERSE_CNX_REQUEST
 *         
 */
class ntudp_pserver_reverse_t : NEOIP_COPY_CTOR_DENY, private udp_full_cb_t {
private:
	ntudp_pserver_t *	ntudp_pserver;	//!< backpointer to the ntudp_pserver_t

	/*************** udp_full_t	***************************************/
	udp_full_t *	udp_full;	//!< the udp_full_t on which this connection occurs
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw();
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		recv_reverse_cnx_r2i_ack(pkt_t &pkt)			throw(serial_except_t);
public:
	/*************** ctor/dtor	***************************************/
	ntudp_pserver_reverse_t(ntudp_pserver_t *ntudp_pserver, udp_full_t *udp_full)	throw();
	~ntudp_pserver_reverse_t()							throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PSERVER_REVERSE_HPP__  */




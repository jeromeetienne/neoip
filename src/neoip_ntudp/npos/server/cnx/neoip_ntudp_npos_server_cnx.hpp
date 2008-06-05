/*! \file
    \brief Declaration of the ntudp_npos_server_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_SERVER_CNX_HPP__ 
#define __NEOIP_NTUDP_NPOS_SERVER_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_npos_server.hpp"
#include "neoip_udp_client_cb.hpp"
#include "neoip_pkt.hpp"
#include "neoip_serial.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


// list of forward declaration
class ntudp_nonce_t;

/** \brief handle a given connection received by the ntudp_npos_server_t
 */
class ntudp_npos_server_t::cnx_t : private udp_full_cb_t, private udp_client_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	ntudp_npos_server_t *ntudp_npos_server;	//!< backpointer to the ntudp_npos_server_t

	/*************** udp_full_t	***************************************/
	udp_full_t *	udp_full;	//!< the udp_full_t 
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw();
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	
	/*************** saddrecho function	*******************************/
	bool		recv_saddrecho_request(pkt_t &pkt)			throw(serial_except_t);
	pkt_t		build_saddrecho_reply(const ntudp_nonce_t &request_nonce)	throw();

	/*************** inetreach_request function	***********************/
	bool		recv_inetreach_request(pkt_t &pkt)			throw(serial_except_t);
	pkt_t		build_inetreach_reply(const ntudp_nonce_t &request_nonce)	throw();
	pkt_t		build_inetreach_probe(const ntudp_nonce_t &request_nonce)	throw();
	udp_client_t *	probe_udp_client;	//!< udp_client_t to send the inetreach_probe
	pkt_t		probe_pkt;		//!< the INETREACH_PROBE packet to send
	bool 		neoip_udp_client_event_cb(void *cb_userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw();

	/*************** inetreach_probe function	***********************/
	bool		recv_inetreach_probe(pkt_t &pkt)			throw(serial_except_t);
public:
	/*************** ctor/dtor	***************************************/
	cnx_t(ntudp_npos_server_t *ntudp_npos_server, udp_full_t *udp_full)	throw();
	~cnx_t()							throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_SERVER_CNX_HPP__  */




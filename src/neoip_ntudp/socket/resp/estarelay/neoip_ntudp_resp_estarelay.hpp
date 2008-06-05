/*! \file
    \brief Declaration of the ntudp_resp_estarelay_t
    
*/


#ifndef __NEOIP_NTUDP_RESP_ESTARELAY_HPP__ 
#define __NEOIP_NTUDP_RESP_ESTARELAY_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_resp_estarelay_cb.hpp"
#include "neoip_ntudp_addr.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_udp_client_cb.hpp"
#include "neoip_pkt.hpp"
#include "neoip_serial.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_resp_t;
class	ipport_addr_t;

/** \brief Perform a saddrecho request and notify the result
 */
class ntudp_resp_estarelay_t : NEOIP_COPY_CTOR_DENY, private udp_client_cb_t, private timeout_cb_t {
private:
	ntudp_resp_t *	ntudp_resp;		//!< backpointer on the ntudp_resp_t
	ntudp_nonce_t	client_nonce;		//!< the client nonce included in each request pkt
	ntudp_addr_t	m_local_addr;		//!< the local ntudp_addr_t
	ntudp_addr_t	m_remote_addr;		//!< the remote ntudp_addr_t

	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;	//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;	//!< to periodically send packet over the udp_client_t
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** packet building	*******************************/
	pkt_t		build_estarelay_cnx_reply(const ipport_addr_t &local_addr_pview)const throw();
	pkt_t 		build_estarelay_cnx_r2i_ack()					const throw();

	/*************** udp_client	***************************************/
	udp_client_t *	udp_client;
	bool		neoip_udp_client_event_cb(void *cb_userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw();
	bool		handle_udp_fatal_event()	throw();

	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		recv_estarelay_cnx_i2r_ack(pkt_t &pkt)			throw(serial_except_t);
	bool		recv_error_pkt(pkt_t &pkt)				throw(serial_except_t);

	/*************** Callback	***************************************/
	ntudp_resp_estarelay_cb_t*callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(udp_full_t *udp_full, const pkt_t &estapkt_in
						, const pkt_t &estapkt_out)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_resp_estarelay_t(ntudp_resp_t *ntudp_resp, ntudp_resp_estarelay_cb_t *callback
							, void * userptr)	throw();
	~ntudp_resp_estarelay_t()				throw();

	/*************** Setup Function	***************************************/
	datum_t	start(const datum_t &request_datum)	throw();

	/*************** Query Function	***************************************/
	const ntudp_addr_t &	local_addr()	const throw()	{ return m_local_addr;	}
	const ntudp_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RESP_ESTARELAY_HPP__  */




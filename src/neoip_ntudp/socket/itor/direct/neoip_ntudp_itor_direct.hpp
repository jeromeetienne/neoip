/*! \file
    \brief Declaration of the ntudp_itor_direct_t
    
*/


#ifndef __NEOIP_NTUDP_ITOR_DIRECT_HPP__ 
#define __NEOIP_NTUDP_ITOR_DIRECT_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_itor_direct_cb.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_udp_client_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_pkt.hpp"
#include "neoip_serial.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_itor_t;

/** \brief Perform a saddrecho request and notify the result
 */
class ntudp_itor_direct_t : NEOIP_COPY_CTOR_DENY, private udp_client_cb_t, private timeout_cb_t {
private:
	ntudp_itor_t *	ntudp_itor;		//!< backpointer on the ntudp_itor_t
	ntudp_nonce_t	client_nonce;		//!< the client nonce included in each request pkt
	ipport_addr_t	pserver_ipport;		//!< the address of the ntudp_pserver_t
		
	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;		//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;		//!< to periodically send packet over the udp_client_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** packet building	*******************************/
	pkt_t 		build_direct_cnx_request()	const throw();

	/*************** udp_client	***************************************/
	udp_client_t *	udp_client;
	bool		neoip_udp_client_event_cb(void *cb_userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw();

	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		recv_direct_cnx_reply(pkt_t &pkt)			throw(serial_except_t);
	bool		recv_error_pkt(pkt_t &pkt)				throw(serial_except_t);


	/*************** Callback	***************************************/
	ntudp_itor_direct_cb_t*	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(udp_full_t *udp_full, const pkt_t &estapkt_in
						, const pkt_t &estapkt_out)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

public:
	/*************** ctor/dtor	***************************************/
	ntudp_itor_direct_t(ntudp_itor_t *ntudp_itor, const ipport_addr_t &pserver_ipport
					, ntudp_itor_direct_cb_t *callback, void * userptr)	throw();
	~ntudp_itor_direct_t()		throw();

	/*************** Query Function	***************************************/
	ipport_addr_t	get_pserver_ipport()	const throw()	{ return pserver_ipport;	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_ITOR_DIRECT_HPP__  */




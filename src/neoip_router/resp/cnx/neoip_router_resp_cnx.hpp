

#ifndef __NEOIP_ROUTER_RESP_CNX_HPP__ 
#define __NEOIP_ROUTER_RESP_CNX_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_router_resp_cnx_wikidbg.hpp"
#include "neoip_router_resp.hpp"
#include "neoip_router_err.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_ip_addr_inval.hpp"
#include "neoip_pkt.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement neoip_router
 */
class router_resp_cnx_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t, private timeout_cb_t 
				, private wikidbg_obj_t<router_resp_cnx_t, router_resp_cnx_wikidbg_init>
				{
private:
	router_resp_t *	router_resp;		//!< backpointer to the router_resp_t
	ip_addr_t	resp_iaddr_reply;	//!< the inner ip_addr_t which have been chosen for resp
	ip_addr_t	itor_iaddr_reply;	//!< the inner ip_addr_t which have been chosen for itor

	ip_addr_t	pick_inner_addr(const ip_addr_inval_t &avail_addr
					, const ip_addr_t &prefered_addr)	throw();
	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;
	bool 		expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;		//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;		//!< to periodically send packet over the socket_full_t
	bool 		rxmit_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
			
	/*************** Packet building	*******************************/
	pkt_t		build_addr_nego_reply()					throw();
	
	/*************** socket_full_t	***************************************/
	socket_full_t *	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw();
	
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		recv_addr_nego_request(pkt_t &pkt)			throw(serial_except_t);
	bool		recv_addr_nego_ack(pkt_t &pkt)				throw(serial_except_t);

public:
	/*************** ctor/dtor	***************************************/
	router_resp_cnx_t(router_resp_t *router_resp)	throw();
	~router_resp_cnx_t()				throw();

	/*************** Setup Function	***************************************/
	router_err_t	start(socket_full_t *socket_full)	throw();

	/*************** List of friend class	*******************************/
	friend class	router_resp_cnx_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_RESP_CNX_HPP__  */




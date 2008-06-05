

#ifndef __NEOIP_ROUTER_ITOR_CNX_HPP__ 
#define __NEOIP_ROUTER_ITOR_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_router_itor_cnx_wikidbg.hpp"
#include "neoip_router_itor_cnx_cb.hpp"
#include "neoip_router_itor.hpp"
#include "neoip_router_err.hpp"
#include "neoip_socket_client_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	router_itor_t;
class	socket_addr_t;
class	ip_addr_t;

/** \brief Class which implement neoip_router
 */
class router_itor_cnx_t : NEOIP_COPY_CTOR_DENY, private socket_client_cb_t, private timeout_cb_t
			, private wikidbg_obj_t<router_itor_cnx_t, router_itor_cnx_wikidbg_init>
			{
private:
	router_itor_t *	router_itor;		//!< backpointer to the router_itor_t

	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;		//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;		//!< to periodically send the packet
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
			
	/*************** Packet building	*******************************/
	pkt_t		build_addr_nego_request()				throw();
	pkt_t		build_addr_nego_ack(const ip_addr_t &acked_itor_addr
							, const ip_addr_t &acked_resp_addr)	throw();
	
	/*************** socket_client_t	***************************************/
	socket_client_t*socket_client;
	bool		neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
				, const socket_event_t &socket_event)		throw();

	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		recv_addr_nego_reply(pkt_t &pkt)			throw(serial_except_t);

	/*************** Callback	***************************************/
	router_itor_cnx_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(socket_full_t *socket_full
					, const ip_addr_t &local_addr, const ip_addr_t &remote_addr
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
	bool			notify_callback_succeed(socket_full_t *socket_full
					, const ip_addr_t &local_addr, const ip_addr_t &remote_addr
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
	bool			notify_callback_failed()					throw();
public:
	/*************** ctor/dtor	***************************************/
	router_itor_cnx_t(router_itor_t *router_itor)	throw();
	~router_itor_cnx_t()				throw();

	/*************** Setup Function	***************************************/
	router_err_t	start(const socket_addr_t &remote_oaddr, router_itor_cnx_cb_t *callback
							, void *userptr)	throw();
	
	/*************** Query Function ***************************************/
	const socket_addr_t	get_remote_oaddr()	const throw();

	/*************** List of friend class	*******************************/
	friend class	router_itor_cnx_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ITOR_CNX_HPP__  */




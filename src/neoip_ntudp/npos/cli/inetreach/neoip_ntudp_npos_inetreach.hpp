/*! \file
    \brief Declaration of the ntudp_npos_inetreach_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_INETREACH_HPP__ 
#define __NEOIP_NTUDP_NPOS_INETREACH_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_npos_inetreach_cb.hpp"
#include "neoip_ntudp_npos_inetreach_wikidbg.hpp"
#include "neoip_ntudp_npos_server_probe_cb.hpp"
#include "neoip_ntudp_npos_cli_profile.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_udp_client_cb.hpp"
#include "neoip_pkt.hpp"
#include "neoip_serial.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_npos_server_t;
class	ntudp_pserver_pool_t;

/** \brief Perform a inetreach request and notify the result
 */
class ntudp_npos_inetreach_t : NEOIP_COPY_CTOR_DENY, private udp_client_cb_t, private timeout_cb_t
					, private ntudp_npos_server_probe_cb_t
					, private wikidbg_obj_t<ntudp_npos_inetreach_t, ntudp_npos_inetreach_wikidbg_init>
					{
private:
	ntudp_npos_cli_profile_t profile;		//!< the profile attached to this object	
	ntudp_nonce_t		request_nonce;		//!< the client nonce included in each request pkt
	ipport_addr_t		listen_addr_pview;	//!< the probe destination address
	ntudp_npos_server_t *	npos_server;		//!< the ntudp_npos_server_t which will received the 
							//!< INETREACH_PROBE aka the one matching the 
							//!< listen_addr_pview
	ntudp_pserver_pool_t *	pserver_pool;		//!< pointer on the ntudp_pserver_pool_t to nofify
							//!< the reachability.
	size_t			nb_recved_reply;	//!< count the number of received reply


	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;	//!< the delay_t generator for the rxmit
	timeout_t	rxmit_timeout;		//!< to periodically send packet over the udp_client_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** packet building	*******************************/
	pkt_t 		build_pkt_request()	const throw();
	
	/*************** udp_client	***************************************/
	udp_client_t *	udp_client;
	bool		neoip_udp_client_event_cb(void *cb_userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw();

	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		recv_inetreach_reply(pkt_t &pkt)			throw(serial_except_t);

	/*************** inetreach_probe callback	***********************/
	void		neoip_ntudp_npos_server_probe_event_cb(void *cb_userptr
					, ntudp_npos_server_t &cb_ntudp_npos_server)	throw();

	/*************** Callback	***************************************/
	ntudp_npos_inetreach_cb_t *	callback;	//!< the callback to notify result
	void *				userptr;	//!< the userptr associated with the callback
	bool				notify_callback(const ntudp_npos_event_t &ntudp_npos_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_inetreach_t()		throw();
	~ntudp_npos_inetreach_t()		throw();

	/*************** Setup function	***************************************/
	ntudp_npos_inetreach_t &set_profile(const ntudp_npos_cli_profile_t &profile)	throw();
	ntudp_err_t	start(const ipport_addr_t &pserver_addr, const ipport_addr_t &listen_addr_pview
				, ntudp_npos_server_t *npos_server, ntudp_pserver_pool_t *pserver_pool
				, ntudp_npos_inetreach_cb_t *callback, void * userptr)	throw();
				
	/*************** Query function	***************************************/
	ipport_addr_t	get_pserver_addr()	const throw();

	/*************** List of friend classes	*******************************/
	friend class	ntudp_npos_inetreach_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_INETREACH_HPP__  */




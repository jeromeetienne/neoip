/*! \file
    \brief Declaration of the ntudp_npos_saddrecho_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_SADDRECHO_HPP__ 
#define __NEOIP_NTUDP_NPOS_SADDRECHO_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_npos_saddrecho_cb.hpp"
#include "neoip_ntudp_npos_saddrecho_wikidbg.hpp"
#include "neoip_ntudp_npos_cli_profile.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_udp_client_cb.hpp"
#include "neoip_pkt.hpp"
#include "neoip_serial.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_pserver_pool_t;

/** \brief Perform a saddrecho request and notify the result
 */
class ntudp_npos_saddrecho_t : NEOIP_COPY_CTOR_DENY, private udp_client_cb_t, private timeout_cb_t
			, private wikidbg_obj_t<ntudp_npos_saddrecho_t, ntudp_npos_saddrecho_wikidbg_init>
			{
private:
	ntudp_nonce_t	request_nonce;		//!< the client nonce included in each request pkt
	ipport_addr_t	local_addr_pview;	//!< the echoed source address. .is_null() during the 
						//!< the itor phase. 
	ntudp_npos_cli_profile_t profile;	//!< the profile attached to this object
	ntudp_pserver_pool_t *	pserver_pool;	//!< pointer on the ntudp_pserver_pool_t to nofify
						//!< the reachability.

	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;	//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;	//!< to periodically send packet over the udp_client_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();


	/*************** packet building	*******************************/
	pkt_t 		build_pkt_request()	const throw();

	/*************** udp_client	***************************************/
	udp_client_t *	udp_client;
	bool		neoip_udp_client_event_cb(void *cb_userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw();

	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		recv_saddrecho_reply(pkt_t &pkt)			throw(serial_except_t);

	/*************** Callback	***************************************/
	ntudp_npos_saddrecho_cb_t *	callback;	//!< the callback to notify result
	void *				userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const ntudp_npos_event_t &ntudp_npos_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_saddrecho_t()		throw();
	~ntudp_npos_saddrecho_t()		throw();
	
	/*************** Setup function	***************************************/
	ntudp_npos_saddrecho_t &set_profile(const ntudp_npos_cli_profile_t &profile)	throw();	
	ntudp_err_t	set_local_addr(const ipport_addr_t &local_addr)		throw();
	void		set_callback(ntudp_npos_saddrecho_cb_t *callback, void * userptr)	throw();
	ntudp_err_t	start(const ipport_addr_t &pserver_addr, ntudp_pserver_pool_t *pserver_pool
					, ntudp_npos_saddrecho_cb_t *callback, void * userptr)	throw();

	/*************** Query function	***************************************/
	ipport_aview_t	local_addr_aview()	const throw();
	ipport_addr_t	get_pserver_addr()	const throw();
	bool		phase_is_itor()		const throw();

	/*************** List of friend classes	*******************************/
	friend class ntudp_npos_saddrecho_wikidbg_t;	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_SADDRECHO_HPP__  */




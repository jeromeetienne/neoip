/*! \file
    \brief Declaration of the ntudp_itor_estarelay_t
    
*/


#ifndef __NEOIP_NTUDP_ITOR_ESTARELAY_HPP__ 
#define __NEOIP_NTUDP_ITOR_ESTARELAY_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_itor_estarelay_cb.hpp"
#include "neoip_ntudp_itor_estarelay_2pserver_cb.hpp"
#include "neoip_ntudp_itor_estarelay_2peer_cb.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_npos_saddrecho_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_itor_t;

/** \brief Perform a saddrecho request and notify the result
 */
class ntudp_itor_estarelay_t : NEOIP_COPY_CTOR_DENY, private ntudp_npos_saddrecho_cb_t
					, private event_hook_cb_t
					, private ntudp_itor_estarelay_2pserver_cb_t
					, private ntudp_itor_estarelay_2peer_cb_t
					{
private:
	ntudp_itor_t *	ntudp_itor;		//!< backpointer on the ntudp_itor_t
	ntudp_nonce_t	client_nonce;		//!< the client nonce included in each request pkt
	ipport_addr_t	pserver_ipport;		//!< the address of the ntudp_pserver_t
	
	/*************** callback for ntudp_aview_pool_t	***************/
	bool neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level) throw();
	bool	new_aview_subscribed_f;		//!< this is BIG BIG BIG shit due to the fact i 
						//!< have to keep state about whichever im subscribed
						//!< or not... it should be a object creation like other
						//!< stuff - to change the event_hook stuff in order to
						//!< create an object which represent the link itself
						//!< thus when it get created it is linked, and when
						//!< it gets deleted it is unlinked. thus i could
						//!< nipmem_new it and use the null pointer as a mark
						//!< to say it is not allocated	
	void	try_stealing_saddrecho()	throw();

	/*************** ntudp_npos_addrecho_t stuff	***********************/
	ntudp_npos_saddrecho_t *	npos_saddrecho;	//!< the stolen ntudp_npos_addrecho_t
	bool 		neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
						, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();
	ipport_addr_t	get_local_ipport_pview()	const throw();

	/*************** estarelay_2pserver stuff	***********************/
	ntudp_itor_estarelay_2pserver_t*estarelay_2pserver;
	bool neoip_ntudp_itor_estarelay_2pserver_cb(void *cb_userptr
					, ntudp_itor_estarelay_2pserver_t &cb_estarelay_2pserver
					, const ipport_addr_t &remote_peer_ipport)	throw();

	/*************** estarelay_2peer stuff	*******************************/
	ntudp_itor_estarelay_2peer_t *	estarelay_2peer;
	bool neoip_ntudp_itor_estarelay_2peer_cb(void *cb_userptr
					, ntudp_itor_estarelay_2peer_t &cb_ntudp_itor_estarelay_2peer
					, udp_full_t *udp_full
					, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();

	/*************** Callback	***************************************/
	ntudp_itor_estarelay_cb_t*callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(udp_full_t *udp_full, const pkt_t &estapkt_in
						, const pkt_t &estapkt_out)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

public:
	/*************** ctor/dtor	***************************************/
	ntudp_itor_estarelay_t(ntudp_itor_t *ntudp_itor)			throw();
	~ntudp_itor_estarelay_t()						throw();
	
	/*************** Setup Function	***************************************/
	ntudp_err_t	start(const ipport_addr_t &pserver_ipport, ntudp_itor_estarelay_cb_t *callback
							, void * userptr)	throw();
	/*************** Query Function	***************************************/
	ipport_addr_t	get_pserver_ipport()	const throw()	{ return pserver_ipport;	}

	/*************** List of friend	***************************************/
	friend class ntudp_itor_estarelay_2pserver_t;
	friend class ntudp_itor_estarelay_2peer_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_ITOR_ESTARELAY_HPP__  */




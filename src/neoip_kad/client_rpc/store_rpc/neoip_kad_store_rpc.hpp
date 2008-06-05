/*! \file
    \brief Declaration of the kad_store_rpc_t
    
*/


#ifndef __NEOIP_KAD_STORE_RPC_HPP__ 
#define __NEOIP_KAD_STORE_RPC_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_store_rpc_cb.hpp"
#include "neoip_kad_store_rpc_wikidbg.hpp"
#include "neoip_kad_client_rpc_vapi.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_pkt.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;
class	kad_addr_t;
class	cookie_id_t;
class	delay_t;

/** \brief do a STORE_RPC 
 */
class kad_store_rpc_t : NEOIP_COPY_CTOR_DENY, private kad_client_rpc_vapi_t
				, private wikidbg_obj_t<kad_store_rpc_t, kad_store_rpc_wikidbg_init>
				{
private:
	kad_peer_t *		kad_peer;	//!< backpointer on the kad_peer_t
	kad_client_rpc_t *	client_rpc;	//!< the kad_client_rpc_t to handle the part common
						//!< to all RPC clients.

	/*************** Packet Building	*******************************/
	pkt_t			build_request_payload(const kad_recdups_t &kad_recdups
						, const cookie_id_t &cookie_id)	const throw();

	/*************** Packet Reception	*******************************/
	kad_event_t		reply_payload_cb(pkt_t &reply_payload)	throw();

	/*************** Callback	***************************************/
	kad_store_rpc_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_event(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_store_rpc_t()	throw();
	~kad_store_rpc_t()	throw();
	
	/*************** Setup Functoin	***************************************/
	kad_err_t	start(kad_peer_t *kad_peer, const kad_recdups_t &kad_recdups
					, const cookie_id_t &cookie_id
					, const kad_addr_t &remote_addr
					, const delay_t &expire_delay
					, kad_store_rpc_cb_t *callback, void *userptr) 		throw();

	//! Start helper to ease the publication of a single kad_rec_t
	kad_err_t	start(kad_peer_t *kad_peer, const kad_rec_t &kad_rec, const cookie_id_t &cookie_id
					, const kad_addr_t &remote_addr, const delay_t &expire_delay
					, kad_store_rpc_cb_t *callback, void *userptr) 		throw()
			{ return start(kad_peer, kad_recdups_t().update(kad_rec), cookie_id, remote_addr
								, expire_delay, callback, userptr);	}


	/*************** List of friend classes	*******************************/
	friend class	kad_store_rpc_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_STORE_RPC_HPP__ 



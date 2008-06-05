/*! \file
    \brief Declaration of the kad_client_rpc_t
    
*/


#ifndef __NEOIP_KAD_CLIENT_RPC_HPP__ 
#define __NEOIP_KAD_CLIENT_RPC_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_client_rpc_vapi.hpp"
#include "neoip_kad_client_rpc_wikidbg.hpp"
#include "neoip_kad_clicnx_cb.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_kad_pkttype.hpp"
#include "neoip_date.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;
class	kad_recdups_t;
class	pkt_t;

/** \brief do a CLIENT_RPC 
 */
class kad_client_rpc_t : NEOIP_COPY_CTOR_DENY, private kad_clicnx_cb_t
				, private wikidbg_obj_t<kad_client_rpc_t, kad_client_rpc_wikidbg_init> {
private:
	kad_peer_t *	kad_peer;	//!< backpointer on the kad_peer_t
	kad_addr_t	remote_addr;	//!< the remote kad_addr_t to send the RPC to
	kad_nonceid_t	request_nonce;	//!< the request for this nonce
	kad_pkttype_t	reply_pkttype;	//!< the kad_pkttype_t expected in the reply packet header
	date_t		creation_date;	//!< the date of creation of this object

	/*************** kad_clicnx_t	***************************************/
	kad_clicnx_t *	kad_clicnx;	//!< pointer on the clicnx of this rpc
	bool		neoip_kad_clicnx_cb(void *cb_userptr, kad_clicnx_t &cb_kad_clicnx
					, const kad_event_t &kad_event)	throw();

	/*************** Packet Reception	*******************************/
	bool		handle_reply(pkt_t &pkt)	throw();

	/*************** Callback	***************************************/
	kad_client_rpc_vapi_t *	callback;	//!< the callback to notify result
	bool			notify_callback(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_client_rpc_t()	throw();
	~kad_client_rpc_t()	throw();
	
	/*************** Setup Function	***************************************/
	kad_err_t	start(kad_peer_t *kad_peer, const pkt_t &request_payload
					, const kad_pkttype_t &reply_pkttype
					, const kad_pkttype_t &request_pkttype
					, const kad_addr_t &remote_addr, const delay_t &expire_delay
					, kad_client_rpc_vapi_t *callback) 		throw();
	
	/*************** Query Function	***************************************/
	const kad_addr_t &	get_remote_addr() const throw() { return remote_addr;	}

	/*************** List of friend classes	*******************************/
	friend class	kad_client_rpc_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_CLIENT_RPC_HPP__ 



/*! \file
    \brief Declaration of the kad_findsomeval_rpc_t
    
*/


#ifndef __NEOIP_KAD_FINDSOMEVAL_RPC_HPP__ 
#define __NEOIP_KAD_FINDSOMEVAL_RPC_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_findsomeval_rpc_cb.hpp"
#include "neoip_kad_findsomeval_rpc_wikidbg.hpp"
#include "neoip_kad_client_rpc_vapi.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;
class	kad_addr_t;
class	kad_recid_t;
class	kad_keyid_t;
class	cookie_id_t;
class	delay_t;

/** \brief do a FINDSOMEVAL_RPC 
 */
class kad_findsomeval_rpc_t : NEOIP_COPY_CTOR_DENY, private kad_client_rpc_vapi_t
				, private wikidbg_obj_t<kad_findsomeval_rpc_t, kad_findsomeval_rpc_wikidbg_init>
				{
private:
	kad_peer_t *		kad_peer;	//!< backpointer on the kad_peer_t
	kad_client_rpc_t *	client_rpc;	//!< the kad_client_rpc_t to handle the part common
						//!< to all RPC clients.

	/*************** Packet Building	*******************************/
	pkt_t			build_request_payload(const kad_keyid_t &keyid
							, size_t max_nb_record)	const throw();

	/*************** Packet Reception	*******************************/
	kad_event_t		reply_payload_cb(pkt_t &reply_payload)	throw();

	/*************** Callback	***************************************/
	kad_findsomeval_rpc_cb_t *callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_event(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_findsomeval_rpc_t()	throw();
	~kad_findsomeval_rpc_t()	throw();
	
	/*************** Setup Functoin	***************************************/
	kad_err_t	start(kad_peer_t *kad_peer, const kad_keyid_t &keyid, size_t max_nb_record
					, const kad_addr_t &remote_addr, const delay_t &expire_delay
					, kad_findsomeval_rpc_cb_t *callback, void *userptr) 	throw();

	/*************** List of friend classes	*******************************/
	friend class	kad_findsomeval_rpc_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_FINDSOMEVAL_RPC_HPP__ 



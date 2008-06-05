/*! \file
    \brief Header of the \ref kad_rpc_profile_t

*/


#ifndef __NEOIP_KAD_RPC_PROFILE_HPP__ 
#define __NEOIP_KAD_RPC_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class kad_rpc_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! true if it is the ping rpc must be answered, false otherwise
	static const bool		PING_RPC_OK;
	//! true if it is the store rpc must be answered, false otherwise
	static const bool		STORE_RPC_OK;
	//! true if it is the findnode rpc must be answered, false otherwise
	static const bool		FINDNODE_RPC_OK;
	//! true if it is the findsomeval rpc must be answered, false otherwise
	static const bool		FINDSOMEVAL_RPC_OK;
	//! true if it is the findallval rpc must be answered, false otherwise
	static const bool		FINDALLVAL_RPC_OK;
	//! true if it is the delete rpc must be answered, false otherwise
	static const bool		DELETE_RPC_OK;
	//! true if a valid cookie is required to accept store rpc
	static const bool		COOKIE_STORE_OK;
	//! true if a valid cookie is required to accept delete rpc
	static const bool		COOKIE_DELETE_OK;
	/** \brief define the maximum length of a reply packet
	 * 
	 * - Currently it is used only in FINDSOMEVAL replies
	 *   - to avoid generating packet which will likely cause heavy fragmentation 
	 *     (e.g. larger than 1500byte)
	 *   - or even being unsendable (e.g. larger than 64k on IPv4)
	 */
	static const size_t		REPLYPKT_MAXLEN;
	/** \brief true if a request nonce MUST be included in the rpc request, false otherwise
	 * 
	 * - not to put the nonce allow to save the space used by a kad_nonceid_t in each packet
	 * - On the other hand, off-path attackers will be able to forge replies
	 */
	static const bool		REQ_NONCE_IN_PKT_OK;
	/** \brief true if the peerid MUST be included in packet, false otherwise
	 * 
	 * - Not to be multi_peer, allow not to include a kad_peerid_t for the realm id in the packet
	 *   thus it saves space, typically 20-byte per packet.
	 * - TODO analyse the consequences
	 *   - especially when a node keep the same address and goes up/down/up
	 *     before other nodes finds out it went down.
	 *   - what about the republication of the records supposed to be store
	 *     on this node
	 */
	static const bool		DEST_PEERID_IN_PKT_OK;
	/** \brief true if the peerid MUST be included in packet, false otherwise
	 * 
	 * - Not to be multi_peer, allow not to include a kad_peerid_t for the realm id in the packet
	 *   thus it saves space, typically 20-byte per packet.
	 * - TODO analyse the consequences
	 *   - especially when a node keep the same address and goes up/down/up
	 *     before other nodes finds out it went down.
	 *   - what about the republication of the records supposed to be store
	 *     on this node
	 */
	//! true if the realmid MUST be included in the rpc request, false otherwise
	static const bool		REALMID_IN_PKT_OK;	
public:
	/*************** ctor/dtor	***************************************/
	kad_rpc_profile_t()	throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();

	//! return true if the cookie MUST be included in the reply (just a helper function)
	bool	cookie_in_reply_ok()	const throw() { return cookie_store_ok() || cookie_delete_ok(); }
				
	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)							\
	private:var_type		var_name##_val;							\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		kad_rpc_profile_t &var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( bool		, ping_rpc_ok);
	PROFILE_VAR_PLAIN( bool		, store_rpc_ok);
	PROFILE_VAR_PLAIN( bool		, findnode_rpc_ok);
	PROFILE_VAR_PLAIN( bool		, findsomeval_rpc_ok);
	PROFILE_VAR_PLAIN( bool		, findallval_rpc_ok);
	PROFILE_VAR_PLAIN( bool		, delete_rpc_ok);
	PROFILE_VAR_PLAIN( bool		, cookie_store_ok);
	PROFILE_VAR_PLAIN( bool		, cookie_delete_ok);
	PROFILE_VAR_PLAIN( size_t	, replypkt_maxlen);
	PROFILE_VAR_PLAIN( bool		, req_nonce_in_pkt_ok);
	PROFILE_VAR_PLAIN( bool		, dest_peerid_in_pkt_ok);
	PROFILE_VAR_PLAIN( bool		, realmid_in_pkt_ok);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RPC_PROFILE_HPP__  */




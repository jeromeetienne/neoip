/*! \file
    \brief Definition of the \ref kad_rpc_profile_t

*/


/* system include */
/* local include */
#include "neoip_kad_rpc_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_rpc_profile_t constant
const bool	kad_rpc_profile_t::PING_RPC_OK		= true;
const bool	kad_rpc_profile_t::STORE_RPC_OK		= true;
const bool	kad_rpc_profile_t::FINDNODE_RPC_OK	= true;
const bool	kad_rpc_profile_t::FINDSOMEVAL_RPC_OK	= true;
const bool	kad_rpc_profile_t::FINDALLVAL_RPC_OK	= true;
const bool	kad_rpc_profile_t::DELETE_RPC_OK	= true;
const bool	kad_rpc_profile_t::COOKIE_STORE_OK	= true;
const bool	kad_rpc_profile_t::COOKIE_DELETE_OK	= true;
const size_t	kad_rpc_profile_t::REPLYPKT_MAXLEN	= 1200;
const bool	kad_rpc_profile_t::REQ_NONCE_IN_PKT_OK	= true;
const bool	kad_rpc_profile_t::DEST_PEERID_IN_PKT_OK= true;
const bool	kad_rpc_profile_t::REALMID_IN_PKT_OK	= true;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_rpc_profile_t::kad_rpc_profile_t()	throw()
{
	ping_rpc_ok(		PING_RPC_OK);
	store_rpc_ok(		STORE_RPC_OK);
	findnode_rpc_ok(	FINDNODE_RPC_OK);
	findsomeval_rpc_ok(	FINDSOMEVAL_RPC_OK);
	findallval_rpc_ok(	FINDALLVAL_RPC_OK);
	delete_rpc_ok(		DELETE_RPC_OK);
	cookie_store_ok(	COOKIE_STORE_OK);
	cookie_delete_ok(	COOKIE_DELETE_OK);
	replypkt_maxlen(	REPLYPKT_MAXLEN);
	req_nonce_in_pkt_ok(	REQ_NONCE_IN_PKT_OK);
	dest_peerid_in_pkt_ok(	DEST_PEERID_IN_PKT_OK);
	realmid_in_pkt_ok(	REALMID_IN_PKT_OK);
	
	// sanity check - the just built object MUST be check OK
	DBG_ASSERT( check().succeed() );
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
kad_err_t	kad_rpc_profile_t::check()	const throw()
{
	// return no error
	return kad_err_t::OK;
}

NEOIP_NAMESPACE_END


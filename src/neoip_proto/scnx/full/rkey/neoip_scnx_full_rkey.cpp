/*! \file
    \brief Definition of \ref scnx_full_rkey_t

\par Possible Improvements
- currently only the hashchain mode is handled
  - it may be interesting to support diffie-hellman too in order to support
    backward secrecy.
- to handle a timeout for the alternate receive key
  - not sure it is an improvement
*/

/* system include */
/* local include */
#include "neoip_scnx_full_rkey.hpp"
#include "neoip_scnx_full.hpp"
#include "neoip_scnx_pkttype.hpp"
#include "neoip_skey_pkcs5_derivkey.hpp"
#include "neoip_assert.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
scnx_full_rkey_t::scnx_full_rkey_t(scnx_full_t *scnx_full)	throw()
{
	// copy the parameter
	this->scnx_full	= scnx_full;
	// init the rxmit_delaygen
	// TODO put this constant in the profile	
	rxmit_delaygen	= delaygen_t( delaygen_expboff_arg_t().min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(30))
								.random_range(0.2) );
}

/** \brief Destructor
 */
scnx_full_rkey_t::~scnx_full_rkey_t()					throw()
{
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        MISC
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true if a rekeying is in progress, false otherwise
 * 
 * - it use local_nonce special value for this
 * - if local_nonce.is_null() == false then rekeying is in progress, otherwise
 *   it isnt.
 */
bool	scnx_full_rkey_t::is_inprogress()		const throw()
{
	if( local_nonce.is_null() )	return false;
	return true;
}


/** \brief Initiate a rekey
 */
void	scnx_full_rkey_t::initiate()			throw()
{
	// sanity check - rekey MUST NOT be inprogress
	DBG_ASSERT( is_inprogress() == false );
	// get a new local_nonce
	local_nonce	= scnx_nonceid_t::build_random();
	// log to debug
	KLOG_DBG("INITIATE A REKEY local_nonce=" << local_nonce);
	// sanity check - the rxmit_timeout MUST NOT be pending
	DBG_ASSERT( rxmit_timeout.is_running() == false );
	// start the packet retransmition
	rxmit_delaygen.reset();
	rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   rxmit_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 * 
 * - send a REKEY_REQUEST packet
 */
bool	scnx_full_rkey_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{	
	// build a REKEY_REQUEST
	pkt_t	pkt = build_rekey_request();
	// log to debug
	KLOG_DBG("build packet = " << pkt );
	// secure the packet
	scnx_err_t	scnx_err;
	scnx_err	= scnx_full->full_xmit->pkt_from_upper(pkt);
	if( scnx_err.failed() )	return true;	
	// notify the packet to the lower layer
	bool tokeep 	= scnx_full->pkt_to_lower_sync_notify(pkt);
	if( !tokeep ) return false;
	// if the outter connection is reliable, stop the timer
	if( scnx_full->is_reliable_outter_cnx() ){
		rxmit_timeout.stop();
	}else{	// if the outter connection is reliable, init the new delay
		rxmit_timeout.change_period( rxmit_delaygen.pre_inc() );
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle the reception of a REKEY_REQUEST
 * 
 * - this is a packet filter
 */
scnx_err_t	scnx_full_rkey_t::recv_rekey_request(pkt_t &pkt)	throw()
{
	scnx_nonceid_t	remote_nonce;
	scnx_err_t	scnx_err;
	scnx_pkttype_t	pkttype;
	try {
		// read the packet type
		pkt >> pkttype;
		// TODO are you sure about this assert ? is this checked by the caller or something ?
		DBG_ASSERT(pkttype == scnx_pkttype_t::REKEY_REQUEST);
		// read the incoming nonce
		pkt >> remote_nonce;
	}catch(serial_except_t &e){
		return scnx_err_t(scnx_err_t::BOGUS_PKT, "Cant read the rekey_request nonce due to " + e.what());
	}

	// swallow the incoming packet
	pkt	= pkt_t();

// build the reply and send it asynchronously (to avoid reccursive callback)
	// build a reply and put it in the parameter packet as this function is a packet filter
	pkt_t	reply_pkt = build_rekey_reply(remote_nonce);
	// secure the reply packet
	scnx_err = scnx_full->full_xmit->pkt_from_upper(reply_pkt);
	if( scnx_err.failed() )	return scnx_err;
	// notify the reply asynchrounsly via pkt_to_lower event
	scnx_full->pkt_to_lower_async_notify(reply_pkt);
	// log to debug
	KLOG_ERR("build packet = " << reply_pkt );

	// if the incoming remote_nonce is equal to the last_remote_nonce
	// - it is a retransmited REKEY_REQUEST due to a lost REKEY_REPLY
	// => dont derive the key again 
	// => send back a REKEY_REPLY
	if( last_remote_nonce == remote_nonce )	return scnx_err_t::OK;
KLOG_ERR("DO THE REKEY");
	// NOTE: here start the rekeying action.

	// copy the last_remote_nonce
	last_remote_nonce = remote_nonce;

	// if an alternate receive key is still pending, delete it
	if( scnx_full->full_arcv )	nipmem_delete	scnx_full->full_arcv;
	// copy the recv context to the arcv context
	// - NOTE: if a arcv timeout is handled, it should be started here
	scnx_full->full_arcv = scnx_full->full_recv;
	// derive the recv_key
	datum_t	salt = datum_t("scnxrkey");
	skey_pkcs5_derivkey( "sha1/nokey/20", scnx_full->recv_key, scnx_full->recv_key, salt, 1 );
	KLOG_ERR("new recv key is =" << scnx_full->recv_key);
	// Init a new recv context
	scnx_full->full_recv = nipmem_new scnx_full_recv_t(scnx_full, scnx_full->ciph_type
					, scnx_full->auth_type, scnx_full->recv_key);
	return scnx_err_t::OK;
}

/** \brief handle the reception of a REKEY_REPLY
 * 
 * - this is a packet filter
 */
scnx_err_t	scnx_full_rkey_t::recv_rekey_reply(pkt_t &pkt)	throw()
{
	scnx_nonceid_t	nonce_in;
	scnx_pkttype_t	pkttype;

	// if there is no rekeying in progress, ignore the packet
	if( is_inprogress() == false )		return scnx_err_t::OK;
	
	try {
		// read the packet type
		pkt >> pkttype;
		DBG_ASSERT(pkttype == scnx_pkttype_t::REKEY_REPLY);
		// read the incoming nonce
		pkt >> nonce_in;
	}catch(serial_except_t &e){
		return scnx_err_t(scnx_err_t::BOGUS_PKT, "Cant read the rekey_reply nonce due to " + e.what());
	}

	KLOG_DBG("enter remote_nonce=" << nonce_in );
	// check if the incoming nonce is equal to the local_nonce, if not ignore the packet
	if( nonce_in != local_nonce )		return scnx_err_t::OK;

	// derive the xmit_key
	datum_t	salt = datum_t("scnxrkey");
	skey_pkcs5_derivkey( "sha1/nokey/20", scnx_full->xmit_key, scnx_full->xmit_key, salt, 1 );
	KLOG_ERR("new xmit key is =" << scnx_full->xmit_key);
	// init a new xmit context
	nipmem_delete	scnx_full->full_xmit;
	scnx_full->full_xmit = nipmem_new scnx_full_xmit_t(scnx_full, scnx_full->ciph_type
						, scnx_full->auth_type, scnx_full->xmit_key);
	// mark the rekey as completed
	local_nonce = scnx_nonceid_t();
	rxmit_timeout.stop();
	
	return scnx_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build a REKEY_REQUEST packet
 */
pkt_t 	scnx_full_rkey_t::build_rekey_request()		throw()
{
	pkt_t	pkt;
	pkt << scnx_pkttype_t(scnx_pkttype_t::REKEY_REQUEST);
	pkt << local_nonce;
	return pkt;
}

/** \brief build a REKEY_REPLY packet
 */
pkt_t 	scnx_full_rkey_t::build_rekey_reply(const scnx_nonceid_t &remote_nonce)	throw()
{
	pkt_t	pkt;
	pkt << scnx_pkttype_t(scnx_pkttype_t::REKEY_REPLY);
	pkt << remote_nonce;
	return pkt;
}


NEOIP_NAMESPACE_END


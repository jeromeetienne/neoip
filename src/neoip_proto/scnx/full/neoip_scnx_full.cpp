/*! \file
    \brief Definition for the \ref scnx_full_t

*/


/* system include */
/* local include */
#include "neoip_scnx_full.hpp"
#include "neoip_scnx_pkttype.hpp"
#include "neoip_rand.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_skey_pkcs5_derivkey.hpp"
#include "neoip_skey_auth.hpp"
#include "neoip_skey_ciph.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref scnx_full_t constant
#if 1
	//! when the xmit sequence number reach SEQNB_ITOR_REKEY, the local
	//! peers initiate a rekeying.
	const uint32_t	scnx_full_t::SEQNB_ITOR_REKEY	= 0x7FFFFFFF;

	//! When the xmit sequence number reaches SEQNB_SIG_RESERVED, it refuses
	//! to send more data packets if the underlying connection is unreliable.
	//! - this allows to keep some space in order to send signalisation packet, like
	//!   rekeying ones, before running out of sequence number
	const uint32_t	scnx_full_t::SEQNB_SIG_RESERVED	= 0xFFFFFF00;
#else
	// value for debug
	const uint32_t	scnx_full_t::SEQNB_ITOR_REKEY	= 0x5;
	const uint32_t	scnx_full_t::SEQNB_SIG_RESERVED	= 0xFFFFFF00;
#endif
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
scnx_full_t::scnx_full_t( const datum_t &master_key
				, const skey_ciph_type_t &ciph_type, const skey_auth_type_t &auth_type
				, bool is_itor_f, bool reliable_outter_cnx
				, const std::string &local_idname, const std::string &remote_idname
				, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	datum_t		base_salt;
	// sanity check - the base_key datum MUST be secure memory
	DBG_ASSERT( master_key.is_secmem() );

	// copy the parameter
	this->ciph_type			= ciph_type;
	this->auth_type			= auth_type;
	this->reliable_outter_cnx	= reliable_outter_cnx;
	this->local_idname		= local_idname;
	this->remote_idname		= remote_idname;
	this->estapkt_in		= estapkt_in;
	this->estapkt_out		= estapkt_out;
	
	// zero some fields
	full_xmit	= NULL;
	full_recv	= NULL;
	full_rkey	= NULL;
	full_arcv	= NULL;
	callback	= NULL;
		
	// derive the recv_key from the master key
	recv_key	= datum_t(master_key.get_len(), datum_t::SECMEM);
	if( is_itor_f )	base_salt = datum_t("resp2itor");
	else		base_salt = datum_t("itor2resp");
	skey_pkcs5_derivkey("sha1/nokey/20", recv_key, master_key, base_salt, 1);

	// derive the xmit_key from the master key
	xmit_key	= datum_t(master_key.get_len(), datum_t::SECMEM);
	if( is_itor_f )	base_salt = datum_t("itor2resp");
	else		base_salt = datum_t("resp2itor");
	skey_pkcs5_derivkey("sha1/nokey/20", xmit_key, master_key, base_salt, 1);
}

/** \brief Destructor
 */
scnx_full_t::~scnx_full_t()				throw()
{
	// close the xmit and recv contexts
	nipmem_zdelete	full_xmit;
	nipmem_zdelete	full_recv;
	nipmem_zdelete	full_rkey;
	nipmem_zdelete	full_arcv;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	scnx_full_t::to_string()		const throw()
{
	std::ostringstream	oss;
	oss << "ciph=" << ciph_type;
	oss << " auth=" << auth_type;
	oss << " xmit_key=" << xmit_key;
	oss << " recv_key=" << recv_key;
	return oss.str();
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          setup functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
scnx_err_t scnx_full_t::start(scnx_full_cb_t *callback, void *userptr)		throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// open the xmit and recv contexts
	full_xmit = nipmem_new scnx_full_xmit_t(this, ciph_type, auth_type, xmit_key);
	full_recv = nipmem_new scnx_full_recv_t(this, ciph_type, auth_type, recv_key);
	// init the rest
	full_arcv = NULL;
	full_rkey = nipmem_new scnx_full_rkey_t(this);
	// return no error
	return scnx_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                        Query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return true if the outter connection is reliable, false otherwise
 * 
 * - if the underlying connection is not reliable, packets that needs reliability
 *   (e.g scnx_pkttype_t::REKEY_REQUEST) need to be explictly retransmited.
 */
bool	scnx_full_t::is_reliable_outter_cnx()			const throw()
{
	return reliable_outter_cnx;
}


/** \brief return the space overhead for data packet
 */
size_t	scnx_full_t::get_mtu_overhead()					throw()
{
	size_t		overhead = 0;
	// get the packet type overhead
#if NEOIP_SCNX_FULL_INTERNAL_DATATYPE
	overhead	+= bytearray_t().serialize(scnx_pkttype_t(scnx_pkttype_t::SEC_DGRAM)).get_len();
#endif
	// get the security overhead - depending on the previously negociated security options
	overhead += full_xmit->get_mtu_overhead();

	return overhead;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       pkt_to_lower notification
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle the *synchronous* notification of a packet
 * 
 * - NOTE: the packet is sent unmodified.
 *   - so if the packet is to be secure, do it before.
 */
bool	scnx_full_t::pkt_to_lower_sync_notify( pkt_t &pkt )		throw()
{
	// send it
	scnx_event_t	scnx_event = scnx_event_t::build_pkt_to_lower(&pkt);
	return notify_callback(scnx_event);	
}

/** \brief handle the *asynchronous* notification of a packet
 * 
 * - it queue the packet and send it via zerotimer_t
 * - NOTE: the packet is sent unmodified.
 *   - so if the packet is to be secure, do it before.
 */
void	scnx_full_t::pkt_to_lower_async_notify( pkt_t &pkt )		throw()
{
	// queue the packet at the end of the list
	pkt_to_lower_list.push_back(pkt);
	// trigger a zerotimer
	pkt_to_lower_zerotimer.append(this, NULL);
}

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - notify all the pkt_t from pkt_to_lower_list via PKT_TO_LOWER event 
 */
bool	scnx_full_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// go thru the whole list
	while( !pkt_to_lower_list.empty() ){
		// get a copy of the first packet of the list
		pkt_t	pkt_to_send	= pkt_to_lower_list.front();
		// delete it from the list
		pkt_to_lower_list.pop_front();
		// send it
		bool tokeep	= pkt_to_lower_sync_notify(pkt_to_send);
		if( !tokeep )	return false;
	}
	// return tokeep
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       pkt_from_upper/lower
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief handle packet from the upper network layer
 */
scnx_err_t scnx_full_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	scnx_err_t	err;
#if NEOIP_SCNX_FULL_INTERNAL_DATATYPE
	// prepend the pkttype
	// - all this dirty code because i need to prepend a serial, maybe to do a function for it ? :)
	serial_t	serial;
	serial << scnx_pkttype_t(scnx_pkttype_t::SEC_DGRAM);
	pkt.prepend( serial.get_data(), serial.get_len() );
#endif

	// if the xmit sequence number reached the reserved zone and it is a unreliable outter cnx
	// => drop the packet
	// NOTE: if the outter cnx is reliable
	// - if the sequence number is in the reserved zone, the packet will be sent anyway as it cant
	//   be dropped. lets hope the rekeying will succeed before seqnb is exhausted.
	// - if the sequence number is exhausted, the packet will be sent but wont be accepted. so
	//   no packet will be able be exchanged over this connection.
	//   - it needs an external reachbility test to declare this connection closed (e.g reachpeer_t)
	// NOTE: if the outter cnx is reliable or unreliable, this case is to be avoided when possible
	// - so the rekey should be initiated earlier enougth to avoid this case
	DBG_ASSERT( full_xmit );
	if( full_xmit->get_seqnb() >= SEQNB_SIG_RESERVED && !is_reliable_outter_cnx()){
		// swallow the packet to drop it
		pkt = pkt_t();
		// return no error
		return scnx_err_t::OK;
	}

	// if the rekeying should be done and none is in progress, initiate a rekeying
	if( full_xmit->get_seqnb() >= SEQNB_ITOR_REKEY && !full_rkey->is_inprogress() )
		full_rkey->initiate();

	// process the outgoing packet
	err = full_xmit->pkt_from_upper(pkt);
	if( err.failed() ){
		KLOG_ERR("an error occured during packet xmit processing : " << err);
		return err;
	}

	// return no error
	return scnx_err_t::OK;
}

/** \brief handle packet from the lower network layer
 */
scnx_err_t scnx_full_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	scnx_err_t	scnx_err;
	scnx_pkttype_t	pkttype;
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);

	// process the incoming packet with the main receive key
	scnx_err = full_recv->pkt_from_lower(pkt);

	// if a new received packet has been accepted with the main recv key
	// and there is a pending alternative receive key. delete it
	if( scnx_err == scnx_err_t::OK && full_arcv )	nipmem_zdelete full_arcv;

	// if the main receive key failed, try the alternate receive key
	if( scnx_err == scnx_err_t::BAD_FULL_AUTH && full_arcv )
		scnx_err = full_arcv->pkt_from_lower(pkt);

	// if the packet can't be received, and if the incoming packet is equal to the 
	// estapkt_in, then reply estapkt_out
	// - this mechanism is used to rxmit the RESP_AUTH packet dropped by the network
	if( scnx_err.failed() && !estapkt_in.is_null() && estapkt_in == pkt ){
		// swallow the incoming packet
		pkt = pkt_t();
		// send the estapkt_out asynchornously
		pkt_to_lower_async_notify(estapkt_out);
		// return noerror
		return scnx_err_t::OK;
	}

	// if the packet can't be received, return an error
	if( scnx_err.failed() ){
		KLOG_ERR("an error occured during packet recv processing : " << scnx_err);
		return scnx_err;
	}
	
	// NOTE: at this point, the packet is authenticated

	// if there is a estapkt_in/estapkt_out, nullify both
	// - at least one authenticated packet has been received, so the other peer has already
	//   received the estapkt_out and both side consider the connection as established
	if( !estapkt_in.is_null() ){
		estapkt_in	= pkt_t();
		estapkt_out	= pkt_t();
	}
	
	// log to debug
	KLOG_DBG("Post securisation pkt=" << pkt);

	// parse the packet
	try {	// read the pkttype (without consuming)
		pkt.unserial_peek( pkttype );
	}catch(serial_except_t &e){
		return scnx_err_t(scnx_err_t::BOGUS_PKT, "Cant read the packet type due to " + e.what());
	}
	KLOG_DBG("received a pkttype=" << pkttype );
	
	// handle the pkt according to its pkttype
	switch( pkttype.get_value() ){
	case scnx_pkttype_t::REKEY_REQUEST:	return full_rkey->recv_rekey_request(pkt);
	case scnx_pkttype_t::REKEY_REPLY:	return full_rkey->recv_rekey_reply(pkt);
#if NEOIP_SCNX_FULL_INTERNAL_DATATYPE
	case scnx_pkttype_t::SEC_DGRAM:		pkt >> pkttype;	// consume the pkttype
						return scnx_err_t::OK;
	default:				break;
#else
	default:				return scnx_err_t::OK;
#endif
	}

	// if this point is reached, return an error
	return scnx_err_t(scnx_err_t::BOGUS_PKT, "Unknown packet type");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	scnx_full_t::notify_callback(const scnx_event_t &scnx_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_scnx_full_event_cb(userptr, *this, scnx_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END

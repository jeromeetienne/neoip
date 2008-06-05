/*! \file
    \brief Definition of the kad_findsomeval_rpc_t

\par Brief Description
\ref kad_findsomeval_rpc_t implements a FINDSOMEVAL rpc.

*/

/* system include */
/* local include */
#include "neoip_kad_findsomeval_rpc.hpp"
#include "neoip_kad_client_rpc.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_findsomeval_rpc_t::kad_findsomeval_rpc_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero the field
	kad_peer	= NULL;
	client_rpc	= NULL;
}

/** \brief Desstructor
 */
kad_findsomeval_rpc_t::~kad_findsomeval_rpc_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// findsomeval the kad_client_rpc_t if needed
	if( client_rpc )	nipmem_delete	client_rpc;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                    Setup function
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
kad_err_t	kad_findsomeval_rpc_t::start(kad_peer_t *kad_peer
				, const kad_keyid_t &keyid, size_t max_nb_record
				, const kad_addr_t &remote_addr, const delay_t &expire_delay
				, kad_findsomeval_rpc_cb_t *callback, void *userptr) 		throw()
{
	kad_pkttype_t	pkttype(kad_peer->get_profile().pkttype());	
	kad_err_t	kad_err;
	// copy some parameters
	this->kad_peer	= kad_peer;
	this->callback	= callback;
	this->userptr	= userptr;

	// build the packet to send
	pkt_t		request_payload;
	request_payload	= build_request_payload(keyid,  max_nb_record);

	// create the kad_client_t and start it
	client_rpc	= nipmem_new kad_client_rpc_t();
	kad_err 	= client_rpc->start(kad_peer, request_payload
					, pkttype.FINDSOMEVAL_REQUEST(), pkttype.FINDSOMEVAL_REPLY()
					, remote_addr, expire_delay, this);
	if( kad_err.failed() )	return kad_err;

	// update the statistic
	kad_peer->get_stat()->nb_findsomeval_sent++;
	
	// return no error
	return kad_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Packet Building
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Build the request payload (aka without the packet header)
 */
pkt_t	kad_findsomeval_rpc_t::build_request_payload(const kad_keyid_t &keyid
								, size_t max_nb_record)	const throw()
{
	pkt_t	pkt;
	// put the keyid
	pkt << keyid;
	// put the max_nb_record
	pkt << uint32_t(max_nb_record);
	// return the just built payload
	return pkt;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Packet Reception
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Called by kad_client_rpc_t to notify a reply payload
 */
kad_event_t	kad_findsomeval_rpc_t::reply_payload_cb(pkt_t &pkt)	throw()
{
	const kad_rpc_profile_t &	profile = kad_peer->get_profile().rpc();	
	kad_event_t			kad_event;
	// parse the incoming packet
	try {
		uint8_t	record_found;
		// read the 'record_found'
		pkt >> record_found;
		// if record have been found, read it and notify the record event
		if( record_found ){
			kad_recdups_t	kad_recdups;
			uint8_t		has_more_record;
			// get the has_more_record
			pkt	>> has_more_record;
			// get the found records
			pkt 	>> kad_recdups;
			// notify the result
			kad_event = kad_event_t::build_recdups(kad_recdups, has_more_record);
		}else{
			// if no record has been found, read the kad_addr_arr and notify it
			kad_addr_arr_t	kad_addr_arr;
			cookie_id_t	cookie_id;
			// read the replied kad_addr_arr
			pkt >> kad_addr_arr;
			// read the cookie token - if cookie_in_reply_ok()
			if( profile.cookie_in_reply_ok() )	pkt >> cookie_id;
			// notify the result
			kad_event = kad_event_t::build_addr_arr(kad_addr_arr, cookie_id);
		}
	}catch(serial_except_t &e){
		KLOG_ERR("Cant parse incoming packet due to " << e.what() );
		return kad_event_t();
	}
	// notify the kad_event
	return kad_event;
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
bool 	kad_findsomeval_rpc_t::notify_event(const kad_event_t &kad_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_findsomeval_rpc_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been findsomevald, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END




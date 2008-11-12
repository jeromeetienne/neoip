/*! \file
    \brief Class to handle the rtmp_resp_cnx_t

\par Short Description
\ref rtmp_resp_cnx_t parses the handshake for the connection received by rtmp_resp_t
*/

/* system include */
/* local include */
#include "neoip_rtmp_resp_cnx.hpp"
#include "neoip_rtmp_resp.hpp"
#include "neoip_rtmp_full.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
rtmp_resp_cnx_t::rtmp_resp_cnx_t()		throw()
{
	// zero some fields
	rtmp_resp	= NULL;
	socket_full	= NULL;
	m_state		= rtmp_resp_cnx_state_t::WAITING_ITORSYN;
}

/** \brief Destructor
 */
rtmp_resp_cnx_t::~rtmp_resp_cnx_t()		throw()
{
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;
	// unlink this object from the rtmp_resp
	if( rtmp_resp )	rtmp_resp->cnx_unlink(this);
}

/** \brief Autodelete the object and return false to ease readability
 */
bool	rtmp_resp_cnx_t::autodelete()		throw()
{
	nipmem_delete	this;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
rtmp_err_t	rtmp_resp_cnx_t::start(rtmp_resp_t *rtmp_resp, socket_full_t *socket_full)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// copy the parameter
	this->rtmp_resp		= rtmp_resp;
	this->socket_full	= socket_full;
	// link this object to the rtmp_resp
	rtmp_resp->cnx_dolink(this);
	// start the socket_full
	socket_err_t	socket_err;
	socket_err	= socket_full->start(this, NULL);
	if( socket_err.failed() )	return rtmp_err_from_socket(socket_err);
	// return no error
	return rtmp_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when a connection is established
 */
bool	rtmp_resp_cnx_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() )	return autodelete();

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*socket_event.get_recved_data());
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}


/** \brief Handle received data on the cnx_t
 *
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	rtmp_resp_cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);
	// queue the received data to the one already received
	m_recved_data.append(pkt.to_datum(datum_t::NOCOPY));

	switch( m_state.get_value() ){
	case rtmp_resp_cnx_state_t::WAITING_ITORSYN:	return handle_waiting_itorsyn();
	case rtmp_resp_cnx_state_t::WAITING_ITORACK:	return handle_waiting_itorack();
	default:	DBG_ASSERT(0);
	}

	// return 'tokeep'
	return true;
}

/** \brief Handle received data on the cnx_t when in rtmp_resp_cnx_state_t::WAITING_ITORSYN
 *
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	rtmp_resp_cnx_t::handle_waiting_itorsyn()	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// check if all needed data have been received
	if( m_recved_data.length() < 1 + rtmp_resp_t::HANDSHAKE_PADLEN )	return true;
	// discard the first byte
	m_recved_data.head_free(1);
	// read the itor_padding
	datum_t	itor_padding;
	itor_padding	= m_recved_data.head_consume(rtmp_resp_t::HANDSHAKE_PADLEN);
	// send a byte value 3
	socket_full->send("\x03", 1);
	// send a random resp_padding
	datum_t	resp_padding(rtmp_resp_t::HANDSHAKE_PADLEN);
	socket_full->send(resp_padding);
	// send back the itor_padding
	socket_full->send(itor_padding);
	// change the state
	m_state		= rtmp_resp_cnx_state_t::WAITING_ITORACK;

	// return 'tokeep'
	return true;
}

/** \brief Handle received data on the cnx_t when in rtmp_resp_cnx_state_t::WAITING_ITORACK
 *
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	rtmp_resp_cnx_t::handle_waiting_itorack()	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// check if all needed data have been received
	if( m_recved_data.length() < rtmp_resp_t::HANDSHAKE_PADLEN )	return true;
	// discard the resp_padding replied by the client
	m_recved_data.head_free(rtmp_resp_t::HANDSHAKE_PADLEN);

	//
	// here spawn the rtmp_full_t and notify the callback and autodelete
	//

	// backup the object_slotid of the socket_full_t - to be able to return its tokeep value
	slot_id_t	socket_full_slotid	= socket_full->get_object_slotid();
	// build the rtmp_full_t
	rtmp_full_t *	rtmp_full;
	rtmp_full	= nipmem_new rtmp_full_t(socket_full, m_recved_data);
	// mark socket_full as unused - as it is now owned by rtmp_full_t
	socket_full	= NULL;
	// notify the rtmp_full_t
	bool tokeep	= rtmp_resp->notify_callback(rtmp_full);
	if( tokeep )	nipmem_delete	this;
	// if the socket_full_t has no been deleted, so 'tokeep' else return 'dontkeep'
	return object_slotid_tokeep(socket_full_slotid);
}



NEOIP_NAMESPACE_END






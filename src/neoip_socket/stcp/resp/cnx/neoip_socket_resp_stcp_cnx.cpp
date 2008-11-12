/*! \file
    \brief Declaration of the neoip_socket_udp_resp
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_resp_stcp_cnx.hpp"
#include "neoip_socket_full_stcp.hpp"
#include "neoip_socket_profile_stcp.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_tcp_event.hpp"
#include "neoip_slay.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_resp_stcp_cnx_t::socket_resp_stcp_cnx_t(socket_resp_stcp_t *p_resp_stcp
						, tcp_full_t *p_tcp_full)	throw()
{
	// copy parameter
	this->m_resp_stcp	= p_resp_stcp;
	this->m_tcp_full	= p_tcp_full;
	// link the connection to the list
	m_resp_stcp->cnx_dolink( this );
}

/** \brief Destructor
 */
socket_resp_stcp_cnx_t::~socket_resp_stcp_cnx_t()				throw()
{
	// delete m_slay_resp if needed
	nipmem_zdelete	m_slay_resp;
	// delete m_tcp_full if needed
	nipmem_zdelete	m_tcp_full;
	// unlink the connection to the list
	m_resp_stcp->cnx_unlink( this );
}

/** \brief autodelete this object - return false for callback 'dontkeep' convenience
 */
bool	socket_resp_stcp_cnx_t::autodelete(const std::string &reason)		throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
	// autodelete
	nipmem_delete	this;
	// return dontkeep for convenience
	return false;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
socket_err_t	socket_resp_stcp_cnx_t::start()	throw()
{
	inet_err_t	inet_err;
	// start the udp_full
	inet_err		= m_tcp_full->start(this, NULL);
	if( inet_err.failed() )		return socket_err_from_inet(inet_err);
	// configure the tcp_full_t
	// - TODO i dont like those setting
	//   - they interfer with the neutral aspect of socket_itor_t
	m_tcp_full->rcvdata_maxlen( 512*1024 );
	m_tcp_full->xmitbuf_maxlen( 50*1024 );
	m_tcp_full->maysend_tshold( socket_full_t::UNLIMITED );
	// Start a slay_resp_t
	socket_profile_t &	socket_profile	= m_resp_stcp->m_socket_profile;
	socket_profile_stcp_t &	profile_dom	= socket_profile_stcp_t::from_socket(socket_profile);
	slay_profile_t &	slay_profile	= profile_dom.slay();
	slay_resp_arg_t		slay_arg	= slay_resp_arg_t().domain(slay_profile.domain())
								.profile(slay_profile);
	slay_err_t	slay_err;
	m_slay_resp	= nipmem_new slay_resp_t();
	slay_err	= m_slay_resp->start(slay_arg);
	if( slay_err.failed() )	return socket_err_from_slay(slay_err);
	// return no error
	return socket_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    tcp_full_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_full_t when to notify an event
 */
bool	socket_resp_stcp_cnx_t::neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
					, const tcp_event_t &tcp_event)	throw()
{
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( tcp_event.is_full_ok() );

	// if the udp_event_t is fatal, delete this cnx_t
	if( tcp_event.is_fatal() )	return autodelete(tcp_event.to_string());

	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::RECVED_DATA:		return handle_recved_data(*tcp_event.get_recved_data());
	case tcp_event_t::MAYSEND_ON:		return handle_maysend_on();
	default:	DBG_ASSERT( 0 );
	}
	// return 'tokeep'
	return true;
}

/** \brief Handle the socket_event_t::RECVED_DATA event
 *
 * @return a tokeep for the socket_client_t
 */
bool	socket_resp_stcp_cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	slay_err_t	slay_err;
	slay_full_t *	slay_full;
	// log to debug
	KLOG_DBG("enter pkt.size()=" << pkt.size());

	// notify the data to slay_itor_t -
	slay_err	= m_slay_resp->notify_recved_data(pkt, &slay_full);
	if( slay_err.failed() )	return autodelete(slay_err.to_string());
	// xmit_ifneeded
	xmit_ifneeded();
	// if a slay_full_t has been spawned, spawn a socket_full_t
	if( slay_full )	return spawn_socket_full(slay_full);

	// return tokeep
	return true;
}

/** \brief Handle the socket_event_t::MAYSEND_ON event
 */
bool	socket_resp_stcp_cnx_t::handle_maysend_on()		throw()
{
	// xmit_ifneeded
	xmit_ifneeded();
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief xmit more data if needed
 */
void	socket_resp_stcp_cnx_t::xmit_ifneeded()					throw()
{
	pkt_t *	xmit_buffer	= &m_slay_resp->xmit_buffer();
	// log to debug
	KLOG_DBG("enter");

	// sanity check - this function is used IIF the http_reqhd.method() is POST
	DBG_ASSERT( m_tcp_full->xmitbuf_freelen() != socket_full_t::UNLIMITED );
	// compute the length of data to xmit
	size_t len2xmit	= std::min(m_tcp_full->xmitbuf_freelen(), xmit_buffer->length());

	// write the data thru the socket_client_t
	size_t written_len;
	written_len	= m_tcp_full->send( xmit_buffer->range(0, len2xmit, datum_t::NOCOPY) );

	// free written_len byte from m_slay_resp->xmit_buffer();
	xmit_buffer->head_free( written_len );

	// if xmit_buffer not empty,set socket_client->maysend_tshold to half xmitbuf_usedlen
	if(!xmit_buffer->empty())
		m_tcp_full->maysend_tshold(m_tcp_full->xmitbuf_usedlen()/2);
}

/** \brief Spawn a socket_full_t with this slay_full_t
 */
bool	socket_resp_stcp_cnx_t::spawn_socket_full(slay_full_t *slay_full)		throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");

	// backup the object_slotid of the tcp_full_t - to be able to return its tokeep value
	slot_id_t	tcp_full_slotid	= m_tcp_full->get_object_slotid();
	// create the socket_full_stcp_tERR
	socket_full_stcp_t * socket_full_stcp	= nipmem_new socket_full_stcp_t();
	// set ctor_param in socket_full_stcp_t
	socket_err	= socket_full_stcp->set_ctor_param(m_tcp_full, slay_full, m_resp_stcp->profile());
	// mark m_tcp_full as unused - as it is now owned by socket_stcp
	m_tcp_full	= NULL;
	// if it fails, delete the socket_full_stcp_t and notify a CNX_REFUSED
	if( socket_err.failed() ){
		nipmem_zdelete	socket_full_stcp;
		return autodelete("cant start socket_full_t due to " + socket_err.to_string());
	}
	// create the socket_full_t
	socket_full_t *	socket_full	= nipmem_new socket_full_t(socket_full_stcp);
	// build the event to notify
	socket_event_t	socket_event	= socket_event_t::build_cnx_established(socket_full);
	bool		tokeep		= m_resp_stcp->notify_callback(socket_event);
	// if the socket_resp_ntlay_t has not been delete during the callback, auto delete this cnx_t
	// - NOTE: be carefull not to use the object after this line
	if( tokeep )	nipmem_delete	this;

	// if the tcp_full_t has no been deleted, so 'tokeep' else return 'dontkeep'
	return object_slotid_tokeep(tcp_full_slotid);
}


NEOIP_NAMESPACE_END





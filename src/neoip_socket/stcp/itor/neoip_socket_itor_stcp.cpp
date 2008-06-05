/*! \file
    \brief Declaration of the socket_itor_stcp_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_itor_stcp.hpp"
#include "neoip_socket_profile_stcp.hpp"
#include "neoip_socket_peerid_stcp.hpp"
#include "neoip_socket_portid_stcp.hpp"
#include "neoip_socket_full_stcp.hpp"
#include "neoip_socket_helper_stcp.hpp"
#include "neoip_socket_itor.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_tcp.hpp"
#include "neoip_tcp_client.hpp"
#include "neoip_slay.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_itor_vapi_t, socket_itor_stcp_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_itor_stcp_t::socket_itor_stcp_t()						throw()
{
	// zero some fields
	callback	= NULL;
	m_tcp_client	= NULL;
	m_slay_itor	= NULL;
	// init some fields
	m_socket_profile= socket_profile_t(socket_domain_t::STCP);
}

/** \brief Destructor
 */
socket_itor_stcp_t::~socket_itor_stcp_t()						throw()
{
	// delete m_slay_itor if needed
	nipmem_zdelete m_slay_itor;
	// delete m_tcp_client if needed
	nipmem_zdelete m_tcp_client;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the socket_profile
 */
void	socket_itor_stcp_t::set_profile(const socket_profile_t &p_socket_profile)	throw()
{
	// copy the socket_profile_t
	m_socket_profile= p_socket_profile;
	// sanity check - the profile MUST valid
	DBG_ASSERT( profile().check() == socket_err_t::OK );
	// sanity check - the profile MUST be of the same domain
	DBG_ASSERT( domain() == profile().get_domain() );
}

/** \brief set the callback
 */
socket_err_t	socket_itor_stcp_t::setup(const socket_type_t &p_socket_type
				, const socket_addr_t &p_local_addr, const socket_addr_t &p_remote_addr
				, socket_itor_vapi_cb_t *callback, void* userptr)	throw()
{
	// copy the parameter
	this->m_socket_type	= p_socket_type;
	this->m_local_addr	= p_local_addr;
	this->m_remote_addr	= p_remote_addr;
	this->callback		= callback;
	this->userptr		= userptr;
	// sanity check - the socket_domain_t MUST support the socket_type_t
	DBG_ASSERT( domain().support(type()) );	
	// sanity check - the local_addr/remote_addr MUST be of the same socket_domain_t
	DBG_ASSERT( local_addr().is_null() || domain() == local_addr().get_domain() );
	DBG_ASSERT( domain() == remote_addr().get_domain() );
	// return no error
	return socket_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 start() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
socket_err_t	socket_itor_stcp_t::start()					throw()
{
	inet_err_t	inet_err;
	// sanity check - the mandatory parameter MUST be set
	DBG_ASSERT( !remote_addr().is_null() );
	DBG_ASSERT( !type().is_null() );
	DBG_ASSERT(  callback );
	// sanity check - the profile MUST valid
	DBG_ASSERT( profile().check().succeed() );

	// create m_tcp_client_t
	m_tcp_client	= nipmem_new tcp_client_t();

	// convert the socket_profile_t into a tcp_profile_t and pass it to the m_tcp_client_t
	tcp_profile_t	tcp_profile	= socket_helper_stcp_t::socket_to_tcp_profile(profile());
	m_tcp_client->profile( tcp_profile );

	// set the tcp_client_t local address if provided
	if( !local_addr().is_null() ){
		// get the local_ipport
		ipport_addr_t	local_ipport	= socket_helper_stcp_t::ipport_addr(local_addr());
		// set the local address in the tcp_client_t
		m_tcp_client->set_local_addr( local_ipport );
	}

	// start the tcp_client_t
	ipport_addr_t	remote_ipport	= socket_helper_stcp_t::ipport_addr(remote_addr());
	inet_err	= m_tcp_client->start(remote_ipport, this, NULL);
	if( inet_err.failed() )	return socket_err_from_inet(inet_err, "Can't start tcp_client_t due to ");	

	// reread the local_addr() once the socket is bound, in case of dynamic binding
	m_local_addr	= socket_addr_t(domain().to_string() + "://" + m_tcp_client->local_addr().to_string());
	DBG_ASSERT( !local_addr().is_null() );

	// return no error
	return socket_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   	tcp_client_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief callback notified by \ref tcp_client_t when it has a event to notify
 */
bool	socket_itor_stcp_t::neoip_tcp_client_event_cb(void *userptr, tcp_client_t &cb_stcp_client
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be is_client_ok()
	DBG_ASSERT( tcp_event.is_client_ok() );

	// if the underlying connection failed, notify a socket_event_t::CNX_REFUSED
	if( tcp_event.is_fatal() )	return notify_callback_err("CNX_refused due to " + tcp_event.to_string());

	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_ESTABLISHED:	return handle_cnx_established();
	case tcp_event_t::RECVED_DATA:		return handle_recved_data(*tcp_event.get_recved_data());
	case tcp_event_t::MAYSEND_ON:		return handle_maysend_on();
	default:	DBG_ASSERT( 0 );
	}
	return true;
}



/** \brief Handle the socket_event_t::CNX_ESTABLISHED event
 * 
 * @return a tokeep for the socket_client_t
 */
bool	socket_itor_stcp_t::handle_cnx_established()	throw()
{
	slay_err_t	slay_err;
	slay_full_t *	slay_full;
	// log to debug
	KLOG_DBG("enter");

	// configure the tcp_client_t
	// - TODO i dont like those setting
	//   - they interfer with the neutral aspect of socket_itor_t 
	m_tcp_client->rcvdata_maxlen( 512*1024 );
	m_tcp_client->xmitbuf_maxlen( 50*1024 );
	m_tcp_client->maysend_tshold( socket_full_t::UNLIMITED );

	// start a slay_itor_t
	socket_profile_stcp_t &	profile_dom	= socket_profile_stcp_t::from_socket(m_socket_profile);
	slay_profile_t &	slay_profile	= profile_dom.slay();
	slay_itor_arg_t		slay_arg	= slay_itor_arg_t().domain(slay_profile.domain())
								.profile(slay_profile);
	m_slay_itor	= nipmem_new slay_itor_t();
	slay_err	= m_slay_itor->start(slay_arg);
	if( slay_err.failed() )	return notify_callback_err(slay_err);
	
	// notify a empty pkt_t to slay_itor_t to get it started
	slay_err	= m_slay_itor->notify_recved_data(pkt_t(), &slay_full);
	if( slay_err.failed() )	return notify_callback_err(slay_err);
	// xmit_ifneeded
	xmit_ifneeded();
	// if a slay_full_t has been spawned, spawn a socket_full_t
	if( slay_full )	return spawn_socket_full(slay_full);
	// return tokeep
	return true;
}

/** \brief Handle the socket_event_t::RECVED_DATA event
 * 
 * @return a tokeep for the socket_client_t
 */
bool	socket_itor_stcp_t::handle_recved_data(pkt_t &pkt)	throw()
{
	slay_err_t	slay_err;
	slay_full_t *	slay_full;
	// log to debug
	KLOG_DBG("enter pkt.size()=" << pkt.size());

	// notify the data to slay_itor_t - 
	slay_err	= m_slay_itor->notify_recved_data(pkt, &slay_full);
	if( slay_err.failed() )	return notify_callback_err(slay_err);
	// xmit_ifneeded
	xmit_ifneeded();
	// if a slay_full_t has been spawned, spawn a socket_full_t
	if( slay_full )	return spawn_socket_full(slay_full);

	// return tokeep
	return true;
}

/** \brief Handle the socket_event_t::MAYSEND_ON event
 */
bool	socket_itor_stcp_t::handle_maysend_on()		throw()
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
void	socket_itor_stcp_t::xmit_ifneeded()					throw()
{
	pkt_t *	xmit_buffer	= &m_slay_itor->xmit_buffer();
	// log to debug
	KLOG_DBG("enter");
	
	// sanity check - this function is used IIF the http_reqhd.method() is POST
	DBG_ASSERT( m_tcp_client->xmitbuf_freelen() != socket_full_t::UNLIMITED );
	// compute the length of data to xmit 
	size_t len2xmit	= std::min(m_tcp_client->xmitbuf_freelen(), xmit_buffer->length());

	// write the data thru the socket_client_t
	size_t written_len;
	written_len	= m_tcp_client->send( xmit_buffer->range(0, len2xmit, datum_t::NOCOPY) );

	// free written_len byte from m_slay_itor->xmit_buffer();
	xmit_buffer->head_free( written_len );

	// if xmit_buffer not empty,set socket_client->maysend_tshold to half xmitbuf_usedlen
	if(!xmit_buffer->empty())
		m_tcp_client->maysend_tshold(m_tcp_client->xmitbuf_usedlen()/2);
}

/** \brief Spawn a socket_full_t with this slay_full_t
 */
bool	socket_itor_stcp_t::spawn_socket_full(slay_full_t *slay_full)		throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");
	
	// steal the tcp_full_t from the tcp_client_t - after this tcp_client_t MUST be deleted
	tcp_full_t *	tcp_full_stolen	= m_tcp_client->steal_full();
	// delete the tcp_client and mark it unused
	nipmem_zdelete	m_tcp_client;
	// create the socket_full_stcp_t
	socket_full_stcp_t * socket_full_stcp	= nipmem_new socket_full_stcp_t();
	// set ctor_param in socket_full_stcp_t
	socket_err	= socket_full_stcp->set_ctor_param(tcp_full_stolen, slay_full, profile());
	// if it fails, delete the socket_full_stcp_t and notify a CNX_REFUSED
	if( socket_err.failed() ){
		nipmem_zdelete socket_full_stcp;
		return notify_callback_err("cant start socket_full_t due to " + socket_err.to_string());
	}
	// create the socket_full_t
	socket_full_t *	socket_full	= nipmem_new socket_full_t(socket_full_stcp);
	// build the event to notify
	return notify_callback( socket_event_t::build_cnx_established(socket_full) );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_itor_stcp_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "connecting";
	oss << " in " << type();
	if( !local_addr().is_null() )	oss << " from " << local_addr();
	oss << " to " << remote_addr();
	// return the just built string
	return oss.str();	
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief helper to notify an error
 */
bool	socket_itor_stcp_t::notify_callback_err(const std::string &reason)	throw()
{
	// notify a socket_event_t::CNX_REFUSED with this reason
	return notify_callback( socket_event_t::build_cnx_refused(reason) );
}

/** \brief helper to notify an error
 */
bool	socket_itor_stcp_t::notify_callback_err(const slay_err_t &slay_err)	throw()
{
	return notify_callback_err("CNX_refused due to " + slay_err.to_string());
}


/** \brief notify the callback
 */
bool	socket_itor_stcp_t::notify_callback(const socket_event_t &socket_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_socket_itor_vapi_cb(userptr, *this, socket_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END








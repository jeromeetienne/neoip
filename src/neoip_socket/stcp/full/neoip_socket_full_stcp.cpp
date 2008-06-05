/*! \file
    \brief Declaration of the neoip_socket_stcp_full
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_full_stcp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_tcp_event.hpp"
#include "neoip_slay.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_full_vapi_t, socket_full_stcp_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_full_stcp_t::socket_full_stcp_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero from field
	callback	= NULL;
	m_tcp_full	= NULL;
	m_slay_full	= NULL;
	// set the delault value
	m_socket_type	= socket_type_t::STREAM;
}

/** \brief Destructor
 */
socket_full_stcp_t::~socket_full_stcp_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete m_slay_full if needed
	nipmem_zdelete	m_slay_full;
	// delete m_tcp_full if needed
	nipmem_zdelete	m_tcp_full;
}

/** \brief notify the socket_full_t is destroyed
 * 
 * - called when the caller of the socket layer just destroyed the socket_full_t
 */
void socket_full_stcp_t::notify_glue_destruction()		throw()
{
	// simply delete the object - as it has nothing to do anymore (e.g. no linger or close)
	nipmem_delete this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   set parameter
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
void	socket_full_stcp_t::set_callback(socket_full_vapi_cb_t *callback, void *userptr)	throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
}

/** \brief set the parameter
 */
socket_err_t	socket_full_stcp_t::set_ctor_param(tcp_full_t *p_tcp_full, slay_full_t *p_slay_full
					, const socket_profile_t &socket_profile)	throw()
{
	// copy the linger profile 
	this->m_tcp_full	= p_tcp_full;
	this->m_slay_full	= p_slay_full;
	this->m_socket_profile	= socket_profile;
	// return no error
	return socket_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         Setup Fcuntion
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/** \brief Start the operation
 */
socket_err_t	socket_full_stcp_t::start()					throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - tcp_full and slay_full MUST be set
	DBG_ASSERT( m_tcp_full );
	DBG_ASSERT( m_slay_full );
	DBG_ASSERT( callback );

	// update the tcp_full_t callback to point at this object
	m_tcp_full->set_callback(this, NULL);

	// read the local_addr() once the socket is started
	m_local_addr	= socket_addr_t(domain().to_string() + "://" + m_tcp_full->local_addr().to_string());
	DBG_ASSERT( !local_addr().is_null() );
	// read the remote_addr() once the socket is started
	m_remote_addr	= socket_addr_t(domain().to_string() + "://" + m_tcp_full->remote_addr().to_string());
	DBG_ASSERT( !remote_addr().is_null() );

	// set the m_initflush_zerotimer - to flush anydata already received by slay_full_t
	m_initflush_zerotimer.append(this, NULL);
	
	// return no error
	return socket_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       autodelete_zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	socket_full_stcp_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// notify the data to slay_itor_t - 
	slay_err_t	slay_err;
	pkt_t		pkt_out;
	slay_err	= m_slay_full->notify_recved_data(pkt_t(), pkt_out);
	if( slay_err.failed() )	return notify_callback( socket_event_t::build_network_error(slay_err.to_string()) );

	// simply forward the event
	socket_event_t	socket_event;
	socket_event	= socket_event_t::build_recved_data( &pkt_out );
	return notify_callback(socket_event);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			socket_stream_vapi_t accessor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the socket_stream_vapi_t 
 * 
 * - NOTE: it is not inlined as it require to have the declaration of tcp_full_t
 *   and i want to avoid to #include tcp_full_t to reduce compilation time
 */
const socket_stream_vapi_t *	socket_full_stcp_t::stream_vapi()		const throw()
{
	return m_tcp_full;
}

/** \brief Return a pointer on the socket_stream_vapi_t 
 * 
 * - NOTE: it is not inlined as it require to have the declaration of tcp_full_t
 *   and i want to avoid to #include tcp_full_t to reduce compilation time
 */
socket_stream_vapi_t *		socket_full_stcp_t::stream_vapi()		throw()
{
	return m_tcp_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			socket_rate_vapi_t accessor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the socket_rate_vapi_t 
 * 
 * - NOTE: it is not inlined as it require to have the declaration of tcp_full_t
 *   and i want to avoid to #include tcp_full_t to reduce compilation time
 */
const socket_rate_vapi_t*	socket_full_stcp_t::rate_vapi()		const throw()
{
	return m_tcp_full;
}

/** \brief Return a pointer on the socket_rate_vapi_t 
 * 
 * - NOTE: it is not inlined as it require to have the declaration of tcp_full_t
 *   and i want to avoid to #include tcp_full_t to reduce compilation time
 */
socket_rate_vapi_t *		socket_full_stcp_t::rate_vapi()		throw()
{
	return m_tcp_full;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         tcp_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_full_t when to notify an event
 */
bool	socket_full_stcp_t::neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_stcp_full
					, const tcp_event_t &tcp_event)		throw()
{
	socket_event_t	socket_event;
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( tcp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::RECVED_DATA:	
			return handle_recved_data(*tcp_event.get_recved_data());
	case tcp_event_t::CNX_CLOSED:{
			// simply forward the event
			std::string	reason	= tcp_event.get_cnx_closed_reason();
			socket_event	= socket_event_t::build_network_error(reason);
			return notify_callback( socket_event );}
	case tcp_event_t::MAYSEND_ON:{
			// simply forward the event
			socket_event	= socket_event_t::build_maysend_on();
			return notify_callback( socket_event );}
	default:	DBG_ASSERT( 0 );
	}

	return true;
}

/** \brief Handle the socket_event_t::RECVED_DATA event
 * 
 * @return a tokeep for the socket_full_t
 */
bool	socket_full_stcp_t::handle_recved_data(pkt_t &pkt_inp)	throw()
{
	// log to debug
	KLOG_DBG("enter pkt_inp.size()=" << pkt_inp.size());

	// notify the data to slay_itor_t - 
	slay_err_t	slay_err;
	pkt_t		pkt_out;
	slay_err	= m_slay_full->notify_recved_data(pkt_inp, pkt_out);
	if( slay_err.failed() )	return notify_callback( socket_event_t::build_network_error(slay_err.to_string()) );

	// simply forward the event
	socket_event_t	socket_event;
	socket_event	= socket_event_t::build_recved_data( &pkt_out );
	return notify_callback(socket_event);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          rcvdata_maxlen function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void	socket_full_stcp_t::rcvdata_maxlen(size_t new_value)	throw()
{
	// pass the function to tcp_full_t	
	m_tcp_full->rcvdata_maxlen(new_value);
}

size_t	socket_full_stcp_t::rcvdata_maxlen()			const throw()
{
	// pass the function to tcp_full_t	
	return m_tcp_full->rcvdata_maxlen();
}

/** \brief write data on the \ref socket_full_stcp_t
 */
size_t	socket_full_stcp_t::send(const void *data_ptr, size_t data_len)	throw()
{
	slay_err_t	slay_err;
	pkt_t		pkt_inp;
	pkt_t		pkt_out;
	// build the input pkt_t
	pkt_inp.work_on_data_nocopy(data_ptr, data_len);
	// notify the recved data to slay_full_t
	slay_err	= m_slay_full->notify_toxmit_data(pkt_inp, pkt_out);
	DBG_ASSERT( slay_err.succeed() );
	// write the data thru the socket_full_t
	size_t		written_len;
	written_len	= m_tcp_full->send( pkt_out );
	// log to debug
	KLOG_DBG("written_len=" << written_len);
	
	// ULTRA DIRTY WORKAROUND
	// - socket_full_t::send return an error if written_len != data_len
	// - and slay_full_t may write more/less than original size
	//   - e.g. in case of slay_domain_t::TLS, it may compress the data and/or 
	//     may add some authentication data
	// - not to disturb anything, m_tcp_full wrote ALL pkt_out, notify a fake
	//   answer with written_len = data_len
	// - i dont like the idea of returning socket_Err_t on send
	//   but the written size is meaningless as it may change or not thru the 
	//   socket.
	if( written_len == pkt_out.size() )	written_len	= data_len;
	
	// return the written_len
	return written_len;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_full_stcp_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "connected";
	oss << " in "	<< type();
	oss << " from "	<< local_addr();
	oss << " to "	<< remote_addr();
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	socket_full_stcp_t::notify_callback(const socket_event_t &socket_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_socket_full_vapi_cb(userptr, *this, socket_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





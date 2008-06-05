/*! \file
    \brief Declaration of the neoip_socket_tcp_full
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_full_tcp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_tcp_event.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_full_vapi_t, socket_full_tcp_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_full_tcp_t::socket_full_tcp_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero from field
	callback	= NULL;
	tcp_full	= NULL;
	// set the delault value
	m_socket_type	= socket_type_t::STREAM;
}

/** \brief Destructor
 */
socket_full_tcp_t::~socket_full_tcp_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destroy internal structure
	nipmem_zdelete	tcp_full;
}

/** \brief notify the socket_full_t is destroyed
 * 
 * - called when the caller of the socket layer just destroyed the socket_full_t
 */
void socket_full_tcp_t::notify_glue_destruction()		throw()
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
void	socket_full_tcp_t::set_callback(socket_full_vapi_cb_t *callback, void *userptr)	throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
}

/** \brief set the parameter
 */
socket_err_t	socket_full_tcp_t::set_ctor_param(tcp_full_t *tcp_full
					, const socket_profile_t &socket_profile)	throw()
{
	// copy the linger profile 
	this->tcp_full		= tcp_full;
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
socket_err_t	socket_full_tcp_t::start()					throw()
{
	inet_err_t	inet_err;
	// sanity check - tcp_full and nlay_full MUST be set
	DBG_ASSERT( tcp_full );
	DBG_ASSERT( callback );

	// start the tcp_full_t
	inet_err	= tcp_full->start(this, NULL);
	if( !inet_err.succeed() )	return socket_err_from_inet(inet_err);

	// read the local_addr() once the socket is started
	m_local_addr	= socket_addr_t(domain().to_string() + "://" + tcp_full->local_addr().to_string());
	DBG_ASSERT( !local_addr().is_null() );
	// read the remote_addr() once the socket is started
	m_remote_addr	= socket_addr_t(domain().to_string() + "://" + tcp_full->remote_addr().to_string());
	DBG_ASSERT( !remote_addr().is_null() );

	// return no error
	return socket_err_t::OK;
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
const socket_stream_vapi_t*	socket_full_tcp_t::stream_vapi()		const throw()
{
	return tcp_full;
}

/** \brief Return a pointer on the socket_stream_vapi_t 
 * 
 * - NOTE: it is not inlined as it require to have the declaration of tcp_full_t
 *   and i want to avoid to #include tcp_full_t to reduce compilation time
 */
socket_stream_vapi_t *		socket_full_tcp_t::stream_vapi()		throw()
{
	return tcp_full;
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
const socket_rate_vapi_t*	socket_full_tcp_t::rate_vapi()		const throw()
{
	return tcp_full;
}

/** \brief Return a pointer on the socket_rate_vapi_t 
 * 
 * - NOTE: it is not inlined as it require to have the declaration of tcp_full_t
 *   and i want to avoid to #include tcp_full_t to reduce compilation time
 */
socket_rate_vapi_t *		socket_full_tcp_t::rate_vapi()		throw()
{
	return tcp_full;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         tcp_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_full_t when to notify an event
 */
bool	socket_full_tcp_t::neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
					, const tcp_event_t &tcp_event)		throw()
{
	socket_event_t	socket_event;
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( tcp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::RECVED_DATA:{
			// simply forward the event
			socket_event = socket_event_t::build_recved_data( tcp_event.get_recved_data() );
			return notify_callback(socket_event);}
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          rcvdata_maxlen function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void	socket_full_tcp_t::rcvdata_maxlen(size_t new_value)	throw()
{
	// pass the function to tcp_full_t	
	tcp_full->rcvdata_maxlen(new_value);
}

size_t	socket_full_tcp_t::rcvdata_maxlen()			const throw()
{
	// pass the function to tcp_full_t	
	return tcp_full->rcvdata_maxlen();
}

/** \brief write data on the \ref socket_full_tcp_t
 */
size_t	socket_full_tcp_t::send(const void *data_ptr, size_t data_len)	throw()
{
	// pass the function to tcp_full_t	
	return tcp_full->send(data_ptr, data_len);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_full_tcp_t::to_string()					const throw()
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
bool	socket_full_tcp_t::notify_callback(const socket_event_t &socket_event)	throw()
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





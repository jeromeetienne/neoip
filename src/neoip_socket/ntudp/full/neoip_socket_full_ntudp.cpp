/*! \file
    \brief Declaration of the neoip_socket_ntudp_full
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_full_ntudp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_full_vapi_t, socket_full_ntudp_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_full_ntudp_t::socket_full_ntudp_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero from field
	callback	= NULL;
	ntudp_full	= NULL;
}

/** \brief Destructor
 */
socket_full_ntudp_t::~socket_full_ntudp_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destroy internal structure
	nipmem_zdelete	ntudp_full;
}

/** \brief notify the socket_full_t is destroyed
 * 
 * - called when the caller of the socket layer just destroyed the socket_full_t
 */
void socket_full_ntudp_t::notify_glue_destruction()		throw()
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
void	socket_full_ntudp_t::set_callback(socket_full_vapi_cb_t *callback, void *userptr)	throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
}

/** \brief set the parameter
 */
socket_err_t	socket_full_ntudp_t::set_ctor_param(ntudp_full_t *ntudp_full
					, const socket_profile_t &socket_profile)	throw()
{
	// copy the linger profile 
	this->ntudp_full	= ntudp_full;
	// copy the socket_profile_t
	m_socket_profile	= socket_profile;
	DBG_ASSERT( profile().check().succeed() );
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
socket_err_t	socket_full_ntudp_t::start()					throw()
{
	ntudp_err_t	ntudp_err;
	// sanity check - ntudp_full and nlay_full MUST be set
	DBG_ASSERT( ntudp_full );
	DBG_ASSERT( callback );

	// start the ntudp_full_t
	ntudp_err	= ntudp_full->start(this, NULL);
	if( !ntudp_err.succeed() )	return socket_err_from_ntudp(ntudp_err);

	// read the local_addr() once the socket is started
	m_local_addr	= socket_addr_t(domain().to_string() + "://" + ntudp_full->local_addr().to_string());
	DBG_ASSERT( !local_addr().is_null() );
	// read the remote_addr() once the socket is started
	m_remote_addr	= socket_addr_t(domain().to_string() + "://" + ntudp_full->remote_addr().to_string());
	DBG_ASSERT( !remote_addr().is_null() );
	// read the socket_type_t once the socket is started
	m_socket_type	= socket_type_t::DGRAM;
	// return no error
	return socket_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			socket_mtu_vapi_t accessor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the socket_mtu_vapi_t 
 * 
 * - NOTE: it is not inlined as it require to have the declaration of tcp_full_t
 *   and i want to avoid to #include tcp_full_t to reduce compilation time
 */
const socket_mtu_vapi_t*	socket_full_ntudp_t::mtu_vapi()		const throw()
{
	return ntudp_full;
}

/** \brief Return a pointer on the socket_mtu_vapi_t 
 * 
 * - NOTE: it is not inlined as it require to have the declaration of tcp_full_t
 *   and i want to avoid to #include tcp_full_t to reduce compilation time
 */
socket_mtu_vapi_t *		socket_full_ntudp_t::mtu_vapi()		throw()
{
	return ntudp_full;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         ntudp_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_full_t when to notify an event
 */
bool	socket_full_ntudp_t::neoip_ntudp_full_event_cb(void *userptr, ntudp_full_t &cb_ntudp_full
					, const ntudp_event_t &ntudp_event)		throw()
{
	socket_event_t	socket_event;
	// log to debug
	KLOG_DBG("enter event=" << ntudp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( ntudp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::RECVED_DATA:{
			// simply forward the event
			socket_event = socket_event_t::build_recved_data( ntudp_event.get_recved_data() );
			return notify_callback(socket_event);}
	case ntudp_event_t::CNX_CLOSED:{
			// simply forward the event
			std::string	reason	= ntudp_event.get_cnx_closed_reason();
			socket_event	= socket_event_t::build_network_error(reason);
			return notify_callback( socket_event );}
	case ntudp_event_t::MTU_CHANGE:
			// simply forward the event
			socket_event	= socket_event_t::build_new_mtu( ntudp_event.get_mtu_change() );
			return notify_callback( socket_event );
	default:	DBG_ASSERT( 0 );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        send function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void	socket_full_ntudp_t::rcvdata_maxlen(size_t new_value)	throw()
{
	ntudp_err_t	ntudp_err;
	ntudp_err	= ntudp_full->recv_max_len_set(new_value);
	DBG_ASSERT( ntudp_err.succeed() );
}

size_t	socket_full_ntudp_t::rcvdata_maxlen()			const throw()
{
	return ntudp_full->recv_max_len_get();
}

/** \brief write data on the \ref socket_full_ntudp_t
 */
size_t	socket_full_ntudp_t::send(const void *data_ptr, size_t data_len)	throw()
{
	// send it thru ntudp_full_t
	return ntudp_full->send(data_ptr, data_len);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_full_ntudp_t::to_string()					const throw()
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
bool	socket_full_ntudp_t::notify_callback(const socket_event_t &socket_event)	throw()
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





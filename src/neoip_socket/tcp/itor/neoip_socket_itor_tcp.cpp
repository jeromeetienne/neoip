/*! \file
    \brief Declaration of the socket_itor_tcp_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_itor_tcp.hpp"
#include "neoip_socket_profile_tcp.hpp"
#include "neoip_socket_peerid_tcp.hpp"
#include "neoip_socket_portid_tcp.hpp"
#include "neoip_socket_full_tcp.hpp"
#include "neoip_socket_helper_tcp.hpp"
#include "neoip_socket_itor.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_tcp.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_itor_vapi_t, socket_itor_tcp_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_itor_tcp_t::socket_itor_tcp_t()						throw()
{
	// zero some fields
	callback	= NULL;
	tcp_itor	= NULL;
	// init some fields
	m_socket_profile= socket_profile_t(socket_domain_t::TCP);
}

/** \brief Destructor
 */
socket_itor_tcp_t::~socket_itor_tcp_t()						throw()
{	
	// destroy insternal structure
	nipmem_zdelete tcp_itor;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the socket_profile
 */
void	socket_itor_tcp_t::set_profile(const socket_profile_t &p_socket_profile)	throw()
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
socket_err_t	socket_itor_tcp_t::setup(const socket_type_t &p_socket_type
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
socket_err_t	socket_itor_tcp_t::start()					throw()
{
	inet_err_t	inet_err;
	// sanity check - the mandatory parameter MUST be set
	DBG_ASSERT( !remote_addr().is_null() );
	DBG_ASSERT( !type().is_null() );
	DBG_ASSERT(  callback );
	// sanity check - the profile MUST valid
	DBG_ASSERT( profile().check().succeed() );

	// create tcp_itor_t
	tcp_itor	= nipmem_new tcp_itor_t();

	// convert the socket_profile_t into a tcp_profile_t and pass it to the tcp_itor_t
	tcp_profile_t	tcp_profile	= socket_helper_tcp_t::socket_to_tcp_profile(profile());
	tcp_itor->profile( tcp_profile );
	
	// set the tcp_itor_t local address if provided
	if( !local_addr().is_null() ){
		// get the local_ipport
		ipport_addr_t	local_ipport	= socket_helper_tcp_t::ipport_addr(local_addr());
		// set the local address in the tcp_itor_t
		tcp_itor->set_local_addr( local_ipport );
	}

	// get the remote_ipport
	ipport_addr_t	remote_ipport	= socket_helper_tcp_t::ipport_addr(remote_addr());
	// start the tcp_itor_t
	inet_err	= tcp_itor->start(remote_ipport, this, NULL);
	if( inet_err.failed() )	return socket_err_from_inet(inet_err, "Can't start tcp_itor_t due to ");	

	// reread the local_addr() once the socket is bound, in case of dynamic binding
	m_local_addr	= socket_addr_t(domain().to_string() + "://" + tcp_itor->local_addr().to_string());
	DBG_ASSERT( !local_addr().is_null() );

	// return no error
	return socket_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   	tcp_itor_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief callback notified by \ref tcp_itor_t when it has a event to notify
 */
bool	socket_itor_tcp_t::neoip_tcp_itor_event_cb(void *userptr, tcp_itor_t &cb_tcp_itor
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be is_itor_ok()
	DBG_ASSERT( tcp_event.is_itor_ok() );

	// if the underlying connection failed, notify a socket_event_t::CNX_REFUSED
	if( tcp_event.is_fatal() ){
		// build the event
		std::string	reason	= "CNX_refused due to " + tcp_event.to_string();
		return notify_callback( socket_event_t::build_cnx_refused(reason) );
	}

	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_ESTABLISHED:{
			tcp_full_t *	tcp_full	= tcp_event.get_cnx_established();
			return recv_cnx_established( tcp_full );}
	default:	DBG_ASSERT( 0 );
	}
	return true;
}

/** \brief callback notified by \ref tcp_itor_t when a connection is established
 * 
 * @return a tokeep for the tcp_itor
 */
bool	socket_itor_tcp_t::recv_cnx_established(tcp_full_t *tcp_full)	throw()
{
	socket_err_t	socket_err;

	// delete the tcp_itor and mark it unused
	nipmem_zdelete	tcp_itor;

	// create the socket_full_tcp_t
	socket_full_tcp_t *	socket_full_tcp	= nipmem_new socket_full_tcp_t();
	// set ctor_param in socket_full_tcp_t
	socket_err	= socket_full_tcp->set_ctor_param(tcp_full, profile());
	// if it fails, delete the nlay_full and socket_full_tcp_t and notify a CNX_REFUSED
	if( socket_err.failed() ){
		nipmem_delete	socket_full_tcp;
		// notify a CNX_REFUSED
		std::string	reason	= "cant start socket_full_t due to " + socket_err.to_string();
		notify_callback( socket_event_t::build_cnx_refused(reason) );
		return false;
	}	
	
	// create the socket_full_t
	socket_full_t *	socket_full	= nipmem_new socket_full_t(socket_full_tcp);

	// build the event to notify
	notify_callback( socket_event_t::build_cnx_established(socket_full) );

	// return dontkeep as the tcp_itor_t has been deleted 
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_itor_tcp_t::to_string()					const throw()
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

/** \brief notify the callback
 */
bool	socket_itor_tcp_t::notify_callback(const socket_event_t &socket_event)	throw()
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








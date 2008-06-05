/*! \file
    \brief Declaration of the \ref socket2_itor_udp_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_itor_udp.hpp"
#include "neoip_socket_profile_udp.hpp"
#include "neoip_socket_peerid_udp.hpp"
#include "neoip_socket_portid_udp.hpp"
#include "neoip_socket_full_udp.hpp"
#include "neoip_socket_layer_udp.hpp"
#include "neoip_socket_itor.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_udp_client.hpp"
#include "neoip_nlay.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_cpp_demangle.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_itor_vapi_t, socket_itor_udp_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_itor_udp_t::socket_itor_udp_t()						throw()
{
	// zero some fields
	callback	= NULL;
	udp_client	= NULL;
	nlay_itor	= NULL;
	// init some fields
	m_socket_profile= socket_profile_t(socket_domain_t::UDP);
	// link this object to the socket_layer_udp_t
	socket_layer_udp_get()->socket_itor_dolink(this);	
}

/** \brief Destructor
 */
socket_itor_udp_t::~socket_itor_udp_t()						throw()
{
	// link this object to the socket_layer_udp_t
	socket_layer_udp_get()->socket_itor_unlink(this);	
	// destroy insternal structure
	nipmem_zdelete nlay_itor;
	nipmem_zdelete udp_client;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the socket_profile
 */
void	socket_itor_udp_t::set_profile(const socket_profile_t &p_socket_profile)	throw()
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
socket_err_t	socket_itor_udp_t::setup(const socket_type_t &p_socket_type
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
	// return noerror
	return socket_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 start() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
socket_err_t	socket_itor_udp_t::start()					throw()
{
	inet_err_t	inet_err;

	// sanity check - the mandatory parameter MUST be set
	DBG_ASSERT( !remote_addr().is_null() );
	DBG_ASSERT( !type().is_null() );
	DBG_ASSERT(  callback );
	// sanity check - the profile MUST valid
	DBG_ASSERT( profile().check().succeed() );

	// create udp_client_t
	udp_client	= nipmem_new udp_client_t();

	// set the udp_client_t local address if provided
	if( !local_addr().is_null() ){
		// get the local_ipport
		ipport_addr_t	local_ipport	= socket_helper_udp_t::ipport_addr(local_addr());
		inet_err	= udp_client->set_local_addr(local_ipport);
		if( inet_err.failed() )
			return socket_err_from_inet(inet_err,"Can't set udp_client_t local address due to ");
	}

	// start the itor_timeout
	itor_timeout.start(profile().itor_timeout(), this, NULL);

	// get the remote_ipport
	ipport_addr_t	remote_ipport	= socket_helper_udp_t::ipport_addr(remote_addr());
	// start the udp_client_t
	inet_err = udp_client->start(remote_ipport, this, NULL);
	if( inet_err.failed() )
		return socket_err_from_inet(inet_err, "Can't start udp_client_t due to ");	

	// create the nlay_itor
	socket_profile_udp_t &	profile_dom	= socket_profile_udp_t::from_socket(m_socket_profile);
	nlay_itor = nipmem_new nlay_itor_t(&profile_dom, type().to_nlay(), nlay_type_t::DGRAM, this, NULL);
#if 0	// TODO here there is a bug as the ASSERT is triggered
	// - udp_client.get_local_addr() return a null ipport... maybe due to the funky way
	//   i do udp_itor_t aka with a zerotimer_t aka no bound in the start()
	// reread the listen_addr() once the socket is bound, in case of dynamic binding
	m_local_addr	= socket_addr_t(domain().to_string() + "://" + udp_client->get_local_addr().to_string());
	KLOG_ERR("local_addr=" << (domain().to_string() + "://" + udp_client->get_local_addr().to_string()));
	DBG_ASSERT( !local_addr().is_null() );
#endif
	// return no error
	return socket_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       	itor_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	socket_itor_udp_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// build the event
	std::string	reason	= "initiator timeed out";
	socket_event_t	socket_event = socket_event_t::build_cnx_refused(reason);
	// TODO CNX_REFUSED event for a itor timeout is not proper
	// notify the event
	return notify_callback(socket_event);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   	udp_client_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief callback notified by \ref udp_client_t when it has a event to notify
 */
bool	socket_itor_udp_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
							, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be is_client_ok()
	DBG_ASSERT( udp_event.is_client_ok() );

	// if the underlying connection failed, notify a socket_event_t::CNX_REFUSED
	if( udp_event.is_fatal() ){
		// build the event
		std::string	reason	= "CNX_refused due to " + udp_event.to_string();
		return notify_callback( socket_event_t::build_cnx_refused(reason) );
	}

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:	return recv_udp_cnx_established();
	case udp_event_t::RECVED_DATA:		return recv_udp_data(*udp_event.get_recved_data());
	default:	DBG_ASSERT( 0 );
	}
	return true;
}

/** \brief callback notified by \ref udp_client_t when a connection is established
 * 
 * @return a tokeep for the udp_client
 */
bool	socket_itor_udp_t::recv_udp_cnx_established()	throw()
{
	nlay_err_t	nlay_err;
	// start the nlay_itor
	nlay_err	= nlay_itor->start();
	// if it failed, report a socket_event_t::CNX_REFUSED
	if( nlay_err.failed() ){
		std::string reason = "Cant start nlay_itor_t due to " + nlay_err.to_string();
		return notify_callback( socket_event_t::build_cnx_refused(reason) );
	}
	// return tokeep
	return true;
}

/** \brief Handle the reception of a udp packet
 * 
 * @return a tokeep for the udp_client_t
 */
bool	socket_itor_udp_t::recv_udp_data(pkt_t &pkt)			throw()
{
	nlay_err_t	nlay_err;
	nlay_event_t	nlay_event;
	// sanity check - the nlay_itor MUST NOT be null
	DBG_ASSERT( nlay_itor );

	// process the incoming packet by nlay_itor
	nlay_err = nlay_itor->pkt_from_lower(pkt, nlay_event);
	if( !nlay_err.succeed() ){
		KLOG_ERR("processing incoming udp packet thru nlay produced "<< nlay_err);
		return true;
	}
	
	// if nlay_itor_t provided a packet to reply, send it back thru udp_client_t
	// - here the udp_full->send error is purposely ignored as udp is an unrealiable outter transport
	//   and anyway nothing can be done as the packet is only a reply from an incoming packet
	if( !pkt.is_null() )	udp_client->send(pkt);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( nlay_event.is_sync_itor_ok() );

	// handle each possible events from its type
	switch( nlay_event.get_value() ){
	case nlay_event_t::CNX_ESTABLISHED:	// if the connection is now established
			// handle the newly established connection and forward the event
			return notify_cnx_established(nlay_event.get_cnx_established());
	case nlay_event_t::CNX_REFUSED:{
			// notify a connection refused
			std::string	reason		= nlay_event.get_cnx_refused_reason();
			return notify_callback( socket_event_t::build_cnx_refused(reason) );}
	case nlay_event_t::NONE:		// if this packet didnt trigger any connection
			break;
	default:	DBG_ASSERT( 0 );
	}

	return true;
}


/** \brief Notify a CNX_ESTABLISHED event 
 * 
 * @return a tokeep for the udp_full_t callback.
 */
bool	socket_itor_udp_t::notify_cnx_established(nlay_full_t *nlay_full)	throw()
{
	socket_err_t	socket_err;

	// steal the udp_full_t from the udp_client_t - after this the udp_client_t MUST be deleted
	udp_full_t *	udp_full_stolen	= udp_client->steal_full();
	// delete the udp_client and mark it unused
	nipmem_zdelete	udp_client;

	// create the socket_full_udp_t
	socket_full_udp_t *	socket_full_udp	= nipmem_new socket_full_udp_t();
	// set ctor_param in socket_full_udp_t
	socket_err	= socket_full_udp->set_ctor_param(udp_full_stolen, nlay_full, profile());
	// if it fails, delete the nlay_full and socket_full_udp and notify a CNX_REFUSED
	if( socket_err.failed() ){
		nipmem_delete	nlay_full;
		nipmem_delete	socket_full_udp;
		// notify a CNX_REFUSED
		std::string	reason	= "cant start socket_full_t due to " + socket_err.to_string();
		notify_callback(socket_event_t::build_cnx_refused(reason));
		return false;
	}	
	
	// create the socket_full_t
	socket_full_t *	socket_full	= nipmem_new socket_full_t(socket_full_udp);
	// build the event to notify
	notify_callback( socket_event_t::build_cnx_established(socket_full) );
	// return dontkeep as the udp_client_t has been deleted 
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      nlay_itor callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Callback notified with \ref nlay_itor_t report an event
 */
bool socket_itor_udp_t::neoip_nlay_itor_event_cb(void *cb_userptr, nlay_itor_t &cb_nlay_itor
					, const nlay_event_t &nlay_event)	throw()
{
	nlay_err_t	nlay_err;
	// log to debug
	KLOG_DBG("enter event=" << nlay_event);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( nlay_event.is_async_itor_ok() );
	// sanity check - the udp_client_t and nlay_itor MUST NOT be null
	DBG_ASSERT( nlay_itor );
	DBG_ASSERT( udp_client );
	
	// handle each possible events from its type
	switch( nlay_event.get_value() ){
	case nlay_event_t::PKT_TO_LOWER:
			// send it thru udp_full_t
			udp_client->send( *nlay_event.get_pkt_to_lower() );
			break;
	default:	DBG_ASSERT( 0 );
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_itor_udp_t::to_string()					const throw()
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
bool	socket_itor_udp_t::notify_callback(const socket_event_t &socket_event)	throw()
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








/*! \file
    \brief Declaration of the socket_resp_stcp_t
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_resp_stcp.hpp"
#include "neoip_socket_resp_stcp_cnx.hpp"
#include "neoip_socket_profile_stcp.hpp"
#include "neoip_socket_peerid_stcp.hpp"
#include "neoip_socket_portid_stcp.hpp"
#include "neoip_socket_full_stcp.hpp"
#include "neoip_socket_helper_stcp.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_tcp_resp.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_tcp_event.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_resp_vapi_t, socket_resp_stcp_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_resp_stcp_t::socket_resp_stcp_t()						throw()
{
	// zero some parameter
	callback	= NULL;
	m_tcp_resp	= NULL;
	// init some fields
	m_socket_profile= socket_profile_t(socket_domain_t::STCP);
}

/** \brief Destructor
 */
socket_resp_stcp_t::~socket_resp_stcp_t()						throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all the pending connection
	while( !cnx_db.empty() )	nipmem_delete	cnx_db.front();
	// delete the tcp_resp_t if needed
	nipmem_zdelete	m_tcp_resp;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the socket_profile
 */
void	socket_resp_stcp_t::set_profile(const socket_profile_t &p_socket_profile)	throw()
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
socket_err_t	socket_resp_stcp_t::setup(const socket_type_t &p_socket_type
				, const socket_addr_t &p_listen_addr
				, socket_resp_vapi_cb_t *callback, void* userptr)	throw()
{
	// copy the parameter
	this->callback		= callback;
	this->userptr		= userptr;
	this->m_socket_type	= p_socket_type;
	this->m_listen_addr	= p_listen_addr;
	// sanity check - the socket_domain_t MUST support the socket_type_t
	DBG_ASSERT( domain().support(type()) );		
	// sanity check - the listen_addr MUST be of the same socket_domain_t
	DBG_ASSERT( domain() == listen_addr().get_domain() );
	// return noerror
	return socket_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
socket_err_t	socket_resp_stcp_t::start()					throw()
{
	inet_err_t	inet_err;

	// sanity check - the mandatory parameter MUST be set
	DBG_ASSERT( !listen_addr().is_null() );
	DBG_ASSERT( !type().is_null() );
	DBG_ASSERT(  callback );

	// sanity check - the profile MUST valid
	DBG_ASSERT( profile().check().succeed() );
	
// SETUP tcp_resp_t
	m_tcp_resp	= nipmem_new tcp_resp_t();

	// convert the socket_profile_t into a tcp_profile_t and pass it to the tcp_resp_t
	tcp_profile_t	tcp_profile	= socket_helper_stcp_t::socket_to_tcp_profile(profile());
	m_tcp_resp->profile( tcp_profile );
	
	// get the listen address
	ipport_addr_t	ipport_addr	= socket_helper_stcp_t::ipport_addr(listen_addr());
	// start the tcp_resp_t
	inet_err	= m_tcp_resp->start(ipport_addr, this, NULL);
	if( inet_err.failed() )
		return socket_err_from_inet(inet_err, "Can't start tcp_resp due to " );

	// reread the listen_addr() once the socket is bound, in case of dynamic binding
	m_listen_addr	= socket_addr_t(domain().to_string() + "://" + m_tcp_resp->listen_addr().to_string());
	DBG_ASSERT( !listen_addr().is_null() );
	
	// return no error	
	return socket_err_t::OK;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   tcp_resp_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_resp_t when a connection is established
 */
bool	socket_resp_stcp_t::neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_stcp_resp
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( tcp_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_ESTABLISHED:
			return recv_cnx_established( tcp_event.get_cnx_established() );
	default:	DBG_ASSERT(0);
	}	
	// return 'tokeep'
	return true;
}

/** \brief callback notified by \ref tcp_client_t when a connection is established
 * 
 * @return a tokeep for the tcp_resp_t
 */
bool	socket_resp_stcp_t::recv_cnx_established(tcp_full_t *tcp_full)	throw()
{
	socket_resp_stcp_cnx_t *	resp_stcp_cnx;
	socket_err_t			socket_err;
	// create a socket_resp_stcp_cnx_t for this tcp_full_t	
	resp_stcp_cnx	= nipmem_new socket_resp_stcp_cnx_t(this, tcp_full);
	socket_err	= resp_stcp_cnx->start();
	if( socket_err.failed() )	nipmem_zdelete	resp_stcp_cnx;
	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       Display Function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_resp_stcp_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "listening on "	<< listen_addr() << " in " << type();
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
bool	socket_resp_stcp_t::notify_callback(const socket_event_t &socket_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_socket_resp_vapi_cb(userptr, *this, socket_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





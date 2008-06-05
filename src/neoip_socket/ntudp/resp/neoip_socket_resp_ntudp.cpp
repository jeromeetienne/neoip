/*! \file
    \brief Declaration of the socket_resp_ntudp_t
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_resp_ntudp.hpp"
#include "neoip_socket_profile_ntudp.hpp"
#include "neoip_socket_peerid_ntudp.hpp"
#include "neoip_socket_portid_ntudp.hpp"
#include "neoip_socket_full_ntudp.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_ntudp_resp.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_resp_vapi_t, socket_resp_ntudp_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_resp_ntudp_t::socket_resp_ntudp_t()						throw()
{
	// zero some parameter
	callback	= NULL;
	ntudp_resp	= NULL;
	// init some fields
	m_socket_profile= socket_profile_t(socket_domain_t::NTUDP);
}

/** \brief Destructor
 */
socket_resp_ntudp_t::~socket_resp_ntudp_t()						throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the ntudp_resp_t if needed
	nipmem_zdelete	ntudp_resp;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the socket_profile
 */
void	socket_resp_ntudp_t::set_profile(const socket_profile_t &p_socket_profile)	throw()
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
socket_err_t	socket_resp_ntudp_t::setup(const socket_type_t &p_socket_type
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
socket_err_t	socket_resp_ntudp_t::start()					throw()
{
	socket_profile_ntudp_t &profile_dom	= socket_profile_ntudp_t::from_socket(m_socket_profile);
	ntudp_peer_t *	ntudp_peer	= profile_dom.ntudp_peer();
	ntudp_err_t	ntudp_err;

	// sanity check - the mandatory parameter MUST be set
	DBG_ASSERT( !listen_addr().is_null() );
	DBG_ASSERT( !type().is_null() );
	DBG_ASSERT(  callback );
	// sanity check - the profile MUST valid
	DBG_ASSERT( profile().check().succeed() );
	
// SETUP ntudp_resp_t
	ntudp_resp	= nipmem_new ntudp_resp_t();
	// get the listen address
	DBG_ASSERT( typeid(*listen_addr().get_peerid_vapi()) == typeid(socket_peerid_ntudp_t));
	DBG_ASSERT( typeid(*listen_addr().get_portid_vapi()) == typeid(socket_portid_ntudp_t));
	ntudp_peerid_t	peerid	= (dynamic_cast <const socket_peerid_ntudp_t &>(*listen_addr().get_peerid_vapi())).ntudp_peerid;
	ntudp_portid_t	portid	= (dynamic_cast <const socket_portid_ntudp_t &>(*listen_addr().get_portid_vapi())).ntudp_portid;
	ntudp_addr_t	ntudp_addr= ntudp_addr_t(peerid, portid);
	// TODO this dynamic_cast is real ugly - they are others - can i hide it ?
	// - with a api specific api trick ? 
	//   - listen_addr().get_peerid().ntudp().get_ntudp_peerid() ?
	// start the ntudp_resp_t
	ntudp_err	= ntudp_resp->start(ntudp_peer, ntudp_addr, this, NULL);
	if( ntudp_err.failed() )
		return socket_err_from_ntudp(ntudp_err, "Can't start ntudp_resp due to " );

	// reread the listen_addr() once the responder is bound, in case of dynamic binding
	m_listen_addr	= socket_addr_t(domain().to_string() + "://" + ntudp_resp->listen_addr().to_string());
	DBG_ASSERT( !listen_addr().is_null() );

	// return no error	
	return socket_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       Display Function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	socket_resp_ntudp_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "listening";
	oss << " on "	<< listen_addr();
	oss << " in "	<< type();
	// return the just built string
	return oss.str();	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   ntudp_resp_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_resp_t when a connection is established
 */
bool	socket_resp_ntudp_t::neoip_ntudp_resp_event_cb(void *userptr, ntudp_resp_t &cb_ntudp_resp
							, const ntudp_event_t &ntudp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << ntudp_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( ntudp_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::CNX_ESTABLISHED:{
			ntudp_full_t *	ntudp_full	= ntudp_event.get_cnx_established();
			return recv_cnx_established( ntudp_full );}
	default:	DBG_ASSERT(0);
	}	
	// return 'tokeep'
	return true;
}

/** \brief callback notified by \ref ntudp_client_t when a connection is established
 * 
 * @return a tokeep for the ntudp_resp_t
 */
bool	socket_resp_ntudp_t::recv_cnx_established(ntudp_full_t *ntudp_full)	throw()
{
	socket_err_t	socket_err;

	// create the socket_full_ntudp_t
	socket_full_ntudp_t *	socket_full_ntudp	= nipmem_new socket_full_ntudp_t();
	// set ctor_param in socket_full_ntudp_t
	socket_err	= socket_full_ntudp->set_ctor_param(ntudp_full, profile());
	// if it fails, delete the socket_full_udp and notify a CNX_REFUSED
	if( socket_err.failed() ){
		nipmem_delete	socket_full_ntudp;
		// notify a CNX_REFUSED
		std::string	reason	= "cant start socket_full_t due to " + socket_err.to_string();
		notify_callback( socket_event_t::build_cnx_refused(reason) );
		return false;
	}	
	
	// create the socket_full_t
	socket_full_t *	socket_full	= nipmem_new socket_full_t(socket_full_ntudp);
	// build the event to notify
	return notify_callback( socket_event_t::build_cnx_established(socket_full) );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	socket_resp_ntudp_t::notify_callback(const socket_event_t &socket_event)	throw()
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





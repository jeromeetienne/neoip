/*! \file
    \brief Declaration of the neoip_socket_udp_resp
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_resp_ntlay.hpp"
#include "neoip_socket_resp_ntlay_cnx.hpp"
#include "neoip_socket_profile_ntlay.hpp"
#include "neoip_socket_peerid_ntlay.hpp"
#include "neoip_socket_portid_ntlay.hpp"
#include "neoip_socket_full_ntlay.hpp"
#include "neoip_socket_layer_ntlay.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_ntudp_resp.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_nlay.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_cpp_demangle.hpp"

NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_resp_vapi_t, socket_resp_ntlay_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_resp_ntlay_t::socket_resp_ntlay_t()						throw()
{
	// zero some parameter
	callback	= NULL;
	ntudp_resp	= NULL;
	nlay_resp	= NULL;
	// init some fields
	m_socket_profile= socket_profile_t(socket_domain_t::NTLAY);
	// link this object to the socket_layer_ntlay_t
	socket_layer_ntlay_get()->socket_resp_dolink(this);		
}

/** \brief Destructor
 */
socket_resp_ntlay_t::~socket_resp_ntlay_t()						throw()
{
	// delete all the pending connection
	while( !cnx_list.empty() )	nipmem_delete	cnx_list.front();
	// delete the udp_resp_t if needed
	nipmem_zdelete	ntudp_resp;
	// delete the nlay_resp_t if needed
	nipmem_zdelete	nlay_resp;
	// unlink this object to the socket_layer_ntlay_t
	socket_layer_ntlay_get()->socket_resp_unlink(this);		
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the socket_profile
 */
void	socket_resp_ntlay_t::set_profile(const socket_profile_t &p_socket_profile)	throw()
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
socket_err_t	socket_resp_ntlay_t::setup(const socket_type_t &p_socket_type
				, const socket_addr_t &p_listen_addr
				, socket_resp_vapi_cb_t *callback, void* userptr)	throw()
{
	// copy the parameter
	this->m_socket_type	= p_socket_type;
	this->m_listen_addr	= p_listen_addr;
	this->callback		= callback;
	this->userptr		= userptr;
	// sanity check -
	DBG_ASSERT( domain() == listen_addr().get_domain() );
	DBG_ASSERT( domain().support(type()) );	
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
socket_err_t	socket_resp_ntlay_t::start()					throw()
{
	socket_profile_ntlay_t &profile_dom	= socket_profile_ntlay_t::from_socket(m_socket_profile);
	ntudp_peer_t *	ntudp_peer	= profile_dom.ntudp_peer();	
	ntudp_err_t	ntudp_err;
	nlay_err_t	nlay_err;
	// sanity check - the mandatory parameter MUST be set
	DBG_ASSERT( !listen_addr().is_null() );
	DBG_ASSERT( !type().is_null() );
	DBG_ASSERT(  callback );
	// sanity check - the profile MUST valid
	DBG_ASSERT( profile().check().succeed() );

// SETUP ntudp_resp_t
	ntudp_resp	= nipmem_new ntudp_resp_t();
	// get the listen address
	DBG_ASSERT( typeid(*listen_addr().get_peerid_vapi()) == typeid(socket_peerid_ntlay_t));
	DBG_ASSERT( typeid(*listen_addr().get_portid_vapi()) == typeid(socket_portid_ntlay_t));
	ntudp_peerid_t	peerid	= (dynamic_cast <const socket_peerid_ntlay_t &>(*listen_addr().get_peerid_vapi())).ntudp_peerid;
	ntudp_portid_t	portid	= (dynamic_cast <const socket_portid_ntlay_t &>(*listen_addr().get_portid_vapi())).ntudp_portid;
	ntudp_addr_t	ntudp_addr= ntudp_addr_t(peerid, portid);
	// TODO this dynamic_cast is real ugly - they are others - can i hide it ?
	// - with a api specific api trick ? 
	//   - listen_addr().get_peerid().ntudp().get_ntudp_peerid() ?
	// start the ntudp_resp_t
	ntudp_err	= ntudp_resp->start(ntudp_peer, ntudp_addr, this, NULL);
	if( ntudp_err.failed() )
		return socket_err_from_ntudp(ntudp_err, "Can't start ntudp_resp_t due to " );

// SETUP NLAY_RESP_T
	// start the nlay_resp_t
	nlay_resp	= nipmem_new nlay_resp_t(&profile_dom, type().to_nlay(), nlay_type_t::DGRAM);
	nlay_err  	= nlay_resp->start();
	if( nlay_err.failed() ){
		nipmem_zdelete	ntudp_resp;
		return socket_err_from_nlay(nlay_err, "Can't start nlay_resp due to ");
	}

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
std::string	socket_resp_ntlay_t::to_string()					const throw()
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
//                   udp_resp_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_resp_t when a connection is established
 */
bool	socket_resp_ntlay_t::neoip_ntudp_resp_event_cb(void *userptr, ntudp_resp_t &cb_ntudp_resp
							, const ntudp_event_t &ntudp_event)	throw()
{
	KLOG_DBG("enter event=" << ntudp_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( ntudp_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::CNX_ESTABLISHED:{
			// spawn a cnx_t which gonna do nlay_resp_t on top on udp_full_t
			ntudp_full_t *	ntudp_full	= ntudp_event.get_cnx_established();
			cnx_t *		cnx		= nipmem_new cnx_t(this, ntudp_full);
			socket_err_t	socket_err	= cnx->start();
			// in case of error, undo the operation
			if( socket_err.failed() ){
				nipmem_delete	cnx;
				nipmem_delete	ntudp_full;
			}
			break;}
	default:	DBG_ASSERT(0);
	}	
	// return 'tokeep'
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	socket_resp_ntlay_t::notify_callback(const socket_event_t &socket_event)	throw()
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





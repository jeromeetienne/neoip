/*! \file
    \brief Declaration of the neoip_socket_udp_resp
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_resp_udp.hpp"
#include "neoip_socket_resp_udp_cnx.hpp"
#include "neoip_socket_profile_udp.hpp"
#include "neoip_socket_peerid_udp.hpp"
#include "neoip_socket_portid_udp.hpp"
#include "neoip_socket_full_udp.hpp"
#include "neoip_socket_layer_udp.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_nlay.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_cpp_demangle.hpp"

NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(socket_resp_vapi_t, socket_resp_udp_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_resp_udp_t::socket_resp_udp_t()						throw()
{
	// zero some parameter
	callback	= NULL;
	udp_resp	= NULL;
	nlay_resp	= NULL;
	// init some fields
	m_socket_profile= socket_profile_t(socket_domain_t::UDP);
	// link this object to the socket_layer_udp_t
	socket_layer_udp_get()->socket_resp_dolink(this);		
}

/** \brief Destructor
 */
socket_resp_udp_t::~socket_resp_udp_t()						throw()
{
	// delete all the pending connection
	while( !cnx_list.empty() )	nipmem_delete	cnx_list.front();
	// delete the udp_resp_t if needed
	nipmem_zdelete	udp_resp;
	// delete the nlay_resp_t if needed
	nipmem_zdelete	nlay_resp;
	// unlink this object to the socket_layer_udp_t
	socket_layer_udp_get()->socket_resp_unlink(this);		
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the socket_profile
 */
void	socket_resp_udp_t::set_profile(const socket_profile_t &p_socket_profile)	throw()
{
	// copy the socket_profile_t
	m_socket_profile	= p_socket_profile;
	// sanity check - the profile MUST be of the same domain
	DBG_ASSERT( domain() == profile().get_domain() );
	// sanity check - the profile MUST valid
	DBG_ASSERT( profile().check().succeed() );
}

/** \brief set the callback
 */
socket_err_t	socket_resp_udp_t::setup(const socket_type_t &p_socket_type
				, const socket_addr_t &p_listen_addr
				, socket_resp_vapi_cb_t *callback, void* userptr)	throw()
{
	// copy the parameter
	this->m_socket_type	= p_socket_type;
	this->m_listen_addr	= p_listen_addr;
	this->callback		= callback;
	this->userptr		= userptr;
	// copy the listen_addr
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
socket_err_t	socket_resp_udp_t::start()					throw()
{
	inet_err_t	inet_err;
	nlay_err_t	nlay_err;
	// sanity check - the mandatory parameter MUST be set
	DBG_ASSERT( !listen_addr().is_null() );
	DBG_ASSERT( !type().is_null() );
	DBG_ASSERT(  callback );
	// sanity check - the profile MUST valid
	DBG_ASSERT( profile().check().succeed() );

// SETUP UDP_RESP_T
	udp_resp	= nipmem_new udp_resp_t();
	// get the listen address
	ipport_addr_t	ipport_addr	= socket_helper_udp_t::ipport_addr(listen_addr());
	// start the udp_resp_t
	inet_err	= udp_resp->start(ipport_addr, this, NULL);
	if( inet_err.failed() )	return socket_err_from_inet(inet_err, "Can't start udp_resp due to " );

	// reread the listen_addr() once the responder is bound, in case of dynamic part
	m_listen_addr	= socket_addr_t(domain().to_string() + "://" + udp_resp->get_listen_addr().to_string());
	DBG_ASSERT( !listen_addr().is_null() );
	
// SETUP NLAY_RESP_T
	// start the nlay_resp_t
	socket_profile_udp_t &	profile_dom	= socket_profile_udp_t::from_socket(m_socket_profile);
	nlay_resp	= nipmem_new nlay_resp_t(&profile_dom, type().to_nlay(), nlay_type_t::DGRAM);
	nlay_err  	= nlay_resp->start();
	if( nlay_err.failed() ){
		nipmem_zdelete udp_resp;
		return socket_err_from_nlay(nlay_err, "Can't start nlay_resp due to ");
	}
	
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
std::string	socket_resp_udp_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "listening on "	<< listen_addr() << " in " << type();
	// return the just built string
	return oss.str();	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   udp_resp_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_resp_t when a connection is established
 */
bool	socket_resp_udp_t::neoip_inet_udp_resp_event_cb(void *userptr, udp_resp_t &cb_udp_resp
							, const udp_event_t &udp_event)	throw()
{
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( udp_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:{
			// spawn a cnx_t which gonna do nlay_resp_t on top on udp_full_t
			udp_full_t *	udp_full	= udp_event.get_cnx_established();
			cnx_t *		cnx		= nipmem_new cnx_t(this, udp_full);
			socket_err_t	socket_err	= cnx->start();
			// in case of error, undo the operation
			if( socket_err.failed() ){
				nipmem_delete	cnx;
				nipmem_delete	udp_full;
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
bool	socket_resp_udp_t::notify_callback(const socket_event_t &socket_event)	throw()
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





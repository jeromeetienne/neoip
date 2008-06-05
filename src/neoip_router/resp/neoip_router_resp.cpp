/*! \file
    \brief Definition of the router_resp_t

\par Brief Description
\ref router_resp_t respond to connection attempts from remote peer.
- description of the address negociation protocol can be found in neoip_router_itor_cnx.cpp
  header.

*/

/* system include */
/* local include */
#include "neoip_router_resp.hpp"
#include "neoip_router_resp_cnx.hpp"
#include "neoip_router_peer.hpp"
#include "neoip_socket.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// include to directly access the socket_profile_t of the socket_domain_t::NTLAY
#include "neoip_socket_profile_ntlay.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_resp_t::router_resp_t(router_peer_t *router_peer)	throw()
{
	// copy the parameter
	this->router_peer	= router_peer;
	// zero some field
	socket_resp		= NULL;
}

/** \brief Destructor
 */
router_resp_t::~router_resp_t()	throw()
{
	// close all pending router_resp_cnx_t
	while(!cnx_db.empty())	nipmem_delete cnx_db.front();
	// delete the socket_resp_t if needed
	nipmem_zdelete	socket_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Setup the router
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
router_err_t router_resp_t::start(const socket_addr_t &listen_addr)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// copy the parameter
	this->listen_addr	= listen_addr;


	// init variable for socket_resp_t
	socket_err_t		socket_err;
	socket_domain_t		socket_domain	= socket_domain_t::NTLAY;
	socket_type_t		socket_type	= socket_type_t::DGRAM;
	socket_profile_t	socket_profile(socket_domain);
	socket_profile_ntlay_t&	profile_dom	= socket_profile_ntlay_t::from_socket(socket_profile);
	profile_dom.scnx().ident_privkey	( router_peer->lident().privkey()	);
	profile_dom.scnx().ident_cert		( router_peer->lident().cert() 		);
	profile_dom.scnx().scnx_auth_ftor	( scnx_auth_ftor_t(router_peer, NULL)	);
	profile_dom.ntudp_peer			( router_peer->ntudp_peer		);
	// init and start the socket_resp_t
	socket_resp	= nipmem_new socket_resp_t();
	socket_err	= socket_resp->setup(socket_domain, socket_type, listen_addr, this, NULL);
	if( socket_err.failed() )	return router_err_from_socket(socket_err);
	socket_err	= socket_resp->set_profile(socket_profile).start();
	if( socket_err.failed() )	return router_err_from_socket(socket_err);

	// return no error
	return router_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_resp_t when a connection is established
 */
bool	router_resp_t::neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
							, const socket_event_t &socket_event)	throw()
{
	router_err_t	router_err;
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( socket_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:{
			// spawn a cnx_t to handle the just-establshed socket_full_t
			router_resp_cnx_t *	resp_cnx;
			resp_cnx	= nipmem_new router_resp_cnx_t(this);
			router_err	= resp_cnx->start( socket_event.get_cnx_established() );
			if( router_err.failed() )
				nipmem_delete resp_cnx;
			break;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END


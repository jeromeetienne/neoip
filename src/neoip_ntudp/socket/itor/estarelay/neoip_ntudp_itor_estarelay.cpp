/*! \file
    \brief Definition of the ntudp_itor_estarelay_t

\par Brief description
This modules handle the itor part for the estarelay connection.

*/

/* system include */
/* local include */
#include "neoip_ntudp_itor_estarelay.hpp"
#include "neoip_ntudp_itor_estarelay_2pserver.hpp"
#include "neoip_ntudp_itor_estarelay_2peer.hpp"
#include "neoip_ntudp_itor.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_aview_pool.hpp"
#include "neoip_ntudp_npos_saddrecho.hpp"
#include "neoip_ntudp_npos_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_itor_estarelay_t::ntudp_itor_estarelay_t(ntudp_itor_t *ntudp_itor)		throw()
{
	// copy the parameter
	this->ntudp_itor	= ntudp_itor;
	// init some variable
	client_nonce		= ntudp_nonce_t::build_random();
	new_aview_subscribed_f	= false;
	estarelay_2pserver	= NULL;
	estarelay_2peer		= NULL;
	// link this object to the ntudp_itor_t
	ntudp_itor->estarelay_link(this);
}

/** \brief Desstructor
 */
ntudp_itor_estarelay_t::~ntudp_itor_estarelay_t()		throw()
{
	ntudp_peer_t *		ntudp_peer	= ntudp_itor->ntudp_peer;	
	ntudp_aview_pool_t *	aview_pool	= ntudp_peer->aview_pool();
	// unlink this object from the ntudp_itor_t
	ntudp_itor->estarelay_unlink(this);
	// unsubscribe the aview_pool if needed
	if( new_aview_subscribed_f )	aview_pool->new_aview_unsubscribe(this, NULL);
	// delete the npos_saddrecho if needed
	if( npos_saddrecho )		nipmem_delete npos_saddrecho;
	// delete the estarelay_pserver if needed
	if( estarelay_2pserver )	nipmem_delete estarelay_2pserver;
	// delete the estarelay_peer if needed
	if( estarelay_2peer )		nipmem_delete estarelay_2peer;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
ntudp_err_t	ntudp_itor_estarelay_t::start(const ipport_addr_t &pserver_ipport
				, ntudp_itor_estarelay_cb_t *callback, void * userptr)	throw()
{
	ntudp_peer_t *		ntudp_peer	= ntudp_itor->ntudp_peer;	
	ntudp_aview_pool_t *	aview_pool	= ntudp_peer->aview_pool();
	// copy the parameter
	this->pserver_ipport	= pserver_ipport;
	this->callback		= callback;
	this->userptr		= userptr;
	// setup the nonce
	client_nonce		= ntudp_nonce_t::build_random();

	// subscribe to aview_pool for new aview
	aview_pool->new_aview_subscribe(this, NULL);
	new_aview_subscribed_f	= true;
	// try to steal saddrecho now
	try_stealing_saddrecho();

	// return no error
	return ntudp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    ntudp_aview_pool_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a event_hook_t level is notified
 * 
 * @return a 'tokeep' for the event_hook_t object
 */
bool	ntudp_itor_estarelay_t::neoip_event_hook_notify_cb(void *userptr
				, const event_hook_t *cb_event_hook, int hook_level)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	
	// sanity check - new_aview_subscribed_f MUST be true here
	DBG_ASSERT( new_aview_subscribed_f );
	
	// try stealing the npos_saddrecho
	try_stealing_saddrecho();
	
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      steal the npos_saddrecho
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Try stealing a npos_saddrecho and start the estarelay_2pserver if it succeed
 */
void	ntudp_itor_estarelay_t::try_stealing_saddrecho()	throw()
{
	ntudp_peer_t *		ntudp_peer	= ntudp_itor->ntudp_peer;	
	ntudp_aview_pool_t *	aview_pool	= ntudp_peer->aview_pool();
	
	// try to steal a ntudp_npos_saddrecho_t from the aview_pool
	npos_saddrecho	= aview_pool->steal_one_saddrecho();
	// if none is present, return now
	if( !npos_saddrecho )	return;
	
	// unsubscribe the aview_pool if needed
	if( new_aview_subscribed_f ){
		aview_pool->new_aview_unsubscribe(this, NULL);
		new_aview_subscribed_f	= false;
	}

	// assign the npos_saddrecho callback to here
	npos_saddrecho->set_callback(this, NULL);	
	
	// launch a ntudp_itor_estarelay_2pserver_t
	DBG_ASSERT( estarelay_2pserver == NULL );
	estarelay_2pserver = nipmem_new ntudp_itor_estarelay_2pserver_t(this, pserver_ipport, this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ntudp_npos_saddrecho_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by ntudp_npos_saddrecho_t when the result is known
 */
bool ntudp_itor_estarelay_t::neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
					, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
					, const ntudp_npos_event_t &ntudp_npos_event)	throw() 
{
	// sanity check - the event MUST be npos_saddrecho_ok
	DBG_ASSERT( ntudp_npos_event.is_npos_saddrecho_ok() );
	// sanity check - the notifier MUST be the local one
	DBG_ASSERT( npos_saddrecho == &cb_ntudp_npos_saddrecho );
	// display the result
	KLOG_DBG("ntudp_npos_saddrecho notified ntudp_npos_event=" << ntudp_npos_event);

	// if an error occured in the npos_saddrecho, notify the faillure to the caller
	if( ntudp_npos_event.is_fatal() )	return notify_callback(NULL, pkt_t(), pkt_t());

	// NOTE: else just maintain the npos_saddrecho until the ntudp_itor_estarelay_2peer_t starts
	
	// return 'tokeep'
	return true;
}

/** \brief Return the local_ipport public view of the ntudp_itor_estarelay_t
 */
ipport_addr_t	ntudp_itor_estarelay_t::get_local_ipport_pview() const throw()
{
	// sanity check - the npos_saddrecho MUST be non null
	DBG_ASSERT( npos_saddrecho != NULL );
	// return the public view of the local address
	return npos_saddrecho->local_addr_aview().pview();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    ntudp_itor_estarelay_2pserver_t callback     
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_itor_estarelay_2pserver_t when the remote_peer_ipport
 *         is known.
 */
bool	ntudp_itor_estarelay_t::neoip_ntudp_itor_estarelay_2pserver_cb(void *cb_userptr
					, ntudp_itor_estarelay_2pserver_t &cb_estarelay_2pserver
					, const ipport_addr_t &remote_peer_ipport)	throw()
{
	// if it failed, report the error to caller
	if( remote_peer_ipport.is_null() )	return notify_callback(NULL, pkt_t(), pkt_t());

	// get the npos_saddrecho local view
	ipport_addr_t	local_ipport	= npos_saddrecho->local_addr_aview().pview();
	// delete the npos_saddrecho and mark it unused
	nipmem_zdelete npos_saddrecho;

	// launch a estarelay_2peer
	DBG_ASSERT( estarelay_2peer == NULL );
	estarelay_2peer = nipmem_new ntudp_itor_estarelay_2peer_t(this, local_ipport, remote_peer_ipport
										, this, NULL);

	// delete estarelay_2pserver and mark it unused
	nipmem_delete estarelay_2pserver;
	estarelay_2pserver	= NULL;

	// return tokeep
	return false;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    ntudp_itor_estarelay_2peer_t callback     
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
					
/** \brief callback notified by \ref ntudp_itor_estarelay_2peer_t when a connection is established
 * 
 */
bool ntudp_itor_estarelay_t::neoip_ntudp_itor_estarelay_2peer_cb(void *cb_userptr
			, ntudp_itor_estarelay_2peer_t &cb_ntudp_itor_estarelay_2peer
			, udp_full_t *udp_full
			, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	// if the estarelay_2peer failed, notify the faillure to the caller
	if( udp_full == NULL )	return notify_callback(NULL, pkt_t(), pkt_t());

	// else notify the connection to the caller
	return notify_callback( udp_full, estapkt_in, estapkt_out);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	ntudp_itor_estarelay_t::notify_callback(udp_full_t *udp_full, const pkt_t &estapkt_in
							, const pkt_t &estapkt_out)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_itor_estarelay_cb(userptr, *this
						, udp_full, estapkt_in, estapkt_out);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - specific to ntudp_itor_estarelay_t, the object MUST be deleted during notification
	DBG_ASSERT( tokeep == false );	
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END



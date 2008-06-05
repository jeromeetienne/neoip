/*! \file
    \brief Definition of the ntudp_npos_natlback_t

\par Notes
- this algorithm works even if the local peer is not inetreach
  - this seems obvious but my first attempts was assuming inetreach OK :)

\par Algorithm
-# discover a public view with saddrecho
-# store the ipport_lview and ipport_pview of the saddrecho and close it
-# bind the ipport_lview with a udp_resp_t
-# start probing the ipport_pview with a nonce
-# if the udp_resp_t receives a connection and that this packets contains the nonce, the local 
  peer is natlback ok, else it isnt

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_natlback.hpp"
#include "neoip_ntudp_npos_natlback_cnx.hpp"
#include "neoip_ntudp_npos_saddrecho.hpp"
#include "neoip_ntudp_npos_event.hpp"
#include "neoip_udp_resp.hpp"
#include "neoip_udp_client.hpp"
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
ntudp_npos_natlback_t::ntudp_npos_natlback_t()		throw()
{
	// setup the nonce
	request_nonce	= ntudp_nonce_t::build_random();	
	// zero some fields
	npos_saddrecho	= NULL;
	udp_resp	= NULL;
	udp_client	= NULL;
}

/** \brief Desstructor
 */
ntudp_npos_natlback_t::~ntudp_npos_natlback_t()		throw()
{
	// close all the cnx_t attached to this udp_vresp_t
	while( !cnx_list.empty() )	nipmem_delete	cnx_list.front();	
	// delete the npos_saddrecho if needed
	if( npos_saddrecho )		nipmem_delete npos_saddrecho;
	// delete the udp_resp if needed
	if( udp_resp )			nipmem_delete udp_resp;
	// delete the udp_client if needed
	if( udp_client )		nipmem_delete udp_client;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         Setup functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_npos_natlback_t &ntudp_npos_natlback_t::set_profile(const ntudp_npos_cli_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief start the actions
 */
ntudp_err_t ntudp_npos_natlback_t::start(const ipport_addr_t &pserver_addr
				, ntudp_pserver_pool_t *pserver_pool
				, ntudp_npos_natlback_cb_t *callback, void * userptr)		throw()
{
	ntudp_err_t	ntudp_err;
	// copy the parameters
	this->pserver_addr	= pserver_addr;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// sanity check - the npos_saddrecho MUST NOT be already initialized
	DBG_ASSERT( npos_saddrecho == NULL );
	// First stage, trigger a ntudp_npos_saddrecho to discover a address lview/pview
	npos_saddrecho	= nipmem_new ntudp_npos_saddrecho_t();
	ntudp_err	= npos_saddrecho->set_profile(profile).start(pserver_addr, pserver_pool
										, this, NULL);
	if( ntudp_err.failed() )	return ntudp_err;

	// return no error
	return ntudp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ntudp_npos_saddrecho_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by ntudp_npos_saddrecho_t when the result is known
 */
bool ntudp_npos_natlback_t::neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
					, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
					, const ntudp_npos_event_t &ntudp_npos_event)	throw() 
{
	inet_err_t	inet_err;
	// sanity check - the event MUST be npos_saddrecho_ok
	DBG_ASSERT( ntudp_npos_event.is_npos_saddrecho_ok() );
	// display the result
	KLOG_DBG("ntudp_npos_saddrecho notified ntudp_npos_event=" << ntudp_npos_event);

	// if the event is fatal, just forward it and return immediatly
	if( ntudp_npos_event.is_fatal() )	return notify_callback(ntudp_npos_event);

	// sanity check - here the event MUST be .is_completed()
	DBG_ASSERT( ntudp_npos_event.is_completed() );

/********************************/
	// backup the result of the npos_saddrecho
	ipport_aview_t	local_addr_aview	= npos_saddrecho->local_addr_aview();
	// delete the object and mark it unused
	nipmem_zdelete	npos_saddrecho;
	
/********************************/
	// sanity check - the udp_resp_t MUST NOT be already initialized
	DBG_ASSERT( udp_resp == NULL );
	// setup the udp_resp_t with the listen_addr_lview
	udp_resp	= nipmem_new udp_resp_t();
	inet_err	= udp_resp->start(local_addr_aview.lview(), this, NULL);
	if( inet_err.failed() ){
		// build a NETWORK_ERROR event and notify it
		ntudp_npos_event_t event = ntudp_npos_event_t::build_network_error(inet_err.to_string());		
		return notify_callback(event);
	}

/********************************/
	// sanity check - the udp_client_t MUST NOT be already initialized
	DBG_ASSERT( udp_client == NULL );
	// launch the udp_client_t to do the probing toward the listen_addr_pview
	udp_client	= nipmem_new udp_client_t();
	inet_err	= udp_client->start(local_addr_aview.pview(), this, NULL);
	if( inet_err.failed() ){
		// build a NETWORK_ERROR event and notify it
		ntudp_npos_event_t event = ntudp_npos_event_t::build_network_error(inet_err.to_string());		
		return notify_callback(event);
	}

	// return 'dontkeep'
	return false;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      udp_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_resp_t when a connection is established
 */
bool	ntudp_npos_natlback_t::neoip_inet_udp_resp_event_cb(void *cb_userptr, udp_resp_t &cb_udp_resp
						, const udp_event_t &udp_event)	throw()
{
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( udp_event.is_resp_ok() );	
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:{
			// if an established connection is notified by the udp_resp_t, create a cnx_t 
			// to handle it 
			// - in peace time, only one is received, the one locally sent
			// - but in war time, an attacker could send fake packet as a DoS
			udp_full_t *	udp_full = udp_event.get_cnx_established();
			// create the connection - it links itself to 'this' udp_vresp_t
			nipmem_new	cnx_t(this, udp_full);
			break;}
	default:	DBG_ASSERT( 0 );	
	}
	// return a 'tokeep'
	return true;
}


/** \brief function callback by cnx_t when a matching nonce is received
 * 
 * @return a tokeep for the cnx_t
 */
bool	ntudp_npos_natlback_t::notify_recved_nonce()	throw()
{
	// build the completed event with true
	ntudp_npos_event_t ntudp_npos_event = ntudp_npos_event_t::build_completed(true);
	// notify the event
	return notify_callback(ntudp_npos_event);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     udp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_client_t when a connection is established
 */
bool	ntudp_npos_natlback_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( udp_event.is_client_ok() );
	
	// handle the fatal events
	if( udp_event.is_fatal() ){
		// if udp_event_t is fatal, notify an COMPLETED event with false
		// - ntudp_npos_natlback_t is special as being unable to reach destination
		//   is part of normal operation and completed and false
		return notify_callback(ntudp_npos_event_t::build_completed(false));
	}

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:
			// init delaygen for the itor phase
			rxmit_delaygen = delaygen_t(profile.cli_itor_pkt_rxmit());
			// start the timer immediatly
			rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
			break;			
	default:	// this connection is not supposed to receive anything, so any other
			// events are ignored/
			break;
	}
	// return 'tokeep'
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback called when the neoip_timeout expire
 */
bool	ntudp_npos_natlback_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("Send a probing nonce");

	// notify the expiration of the timeout to the delaygen
	rxmit_delaygen.notify_expiration();
	// if the delaygen is is_timedout, notify an COMPLETED event with false
	// - ntudp_npos_natlback_t is special as timeing out means completed but false
	if( rxmit_delaygen.is_timedout() )
		return notify_callback(ntudp_npos_event_t::build_completed(false));

	// build the request
	pkt_t	pkt;
	pkt << request_nonce;
	// send the request
	udp_client->send( pkt );

	// set the next timer
	rxmit_timeout.change_period(rxmit_delaygen.pre_inc());

	// return a 'tokeep'
	return true;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep for this object
 */
bool 	ntudp_npos_natlback_t::notify_callback(const ntudp_npos_event_t &ntudp_npos_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_npos_natlback_event_cb(userptr, *this, ntudp_npos_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



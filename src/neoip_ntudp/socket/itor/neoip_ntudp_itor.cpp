/*! \file
    \brief Definition of the ntudp_itor_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_itor.hpp"
#include "neoip_ntudp_itor_direct.hpp"
#include "neoip_ntudp_itor_estarelay.hpp"
#include "neoip_ntudp_itor_reverse.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_ntudp_peer_rec.hpp"
#include "neoip_kad.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_itor_t::ntudp_itor_t()		throw()
{
	// zero some fields
	ntudp_peer	= NULL;
	callback	= NULL;
	kad_query	= NULL;
	profile		= NULL;
}

/** \brief destructor
 */
ntudp_itor_t::~ntudp_itor_t()		throw()
{
	// unlink this object from the ntudp_peer_t
	if( ntudp_peer )	ntudp_peer->ntudp_itor_unlink(this);
	// delete the profile if needed
	nipmem_zdelete	profile;	
	// delete the kad_query_t if needed
	nipmem_zdelete	kad_query;

	// delete all the pending subitor
	delete_all_subitor();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      Setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the local address
 */
ntudp_itor_t &	ntudp_itor_t::set_local_addr(const ntudp_addr_t &m_local_addr)	throw()
{
	// sanity check - the ntudp_itor_t MUST NOT be already started
	DBG_ASSERT( ntudp_peer == NULL );
	// copy some parameter
	this->m_local_addr	= m_local_addr;
	// return the object itself
	return *this;
}

/** \brief Set the callback
 */
ntudp_itor_t &	ntudp_itor_t::set_callback(ntudp_itor_cb_t *callback, void *userptr)	throw()
{
	// copy some parameter
	this->callback		= callback;
	this->userptr		= userptr;
	// return the object itself
	return *this;
}


/** \brief Set the expiration delay
 */
ntudp_itor_t &	ntudp_itor_t::set_timeout(const delay_t &expire_delay)	throw()
{
	// copy some parameter
	this->expire_delay	= expire_delay;
	// return the object itself
	return *this;
}


/** \brief Set the profile for this object
 */
ntudp_itor_t &	ntudp_itor_t::set_profile(const ntudp_itor_profile_t &profile)throw()
{
	// sanity check - the local profile MUST NOT be already set
	DBG_ASSERT( this->profile == NULL );
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );
	// copy the parameter
	this->profile	= nipmem_new ntudp_itor_profile_t(profile);
	// return the object iself
	return *this;
}


/** \brief Set the remote address
 * 
 * - NOTE: this is binding the connection
 *   - if the local_addr is to be specified by the caller, it MUST be before this function.
 */
ntudp_err_t	ntudp_itor_t::set_remote_addr(ntudp_peer_t *ntudp_peer
					, const ntudp_addr_t &m_remote_addr)	throw()
{
	// sanity check - ntudp_peer MUST NOT be null
	DBG_ASSERT( ntudp_peer );
	// -- Set the local address stuff
	// if the local_addr has been NOT been set, set it to a random portid
	if( m_local_addr.is_null() )
		m_local_addr = ntudp_addr_t(ntudp_peer->local_peerid(), ntudp_portid_t::build_random());
	// if the local_addr peerid is not qualified, set it to the ntudp_peer_t one
	if( m_local_addr.peerid().is_null() )	m_local_addr.peerid(ntudp_peer->local_peerid());

	// sanity check - the local_addr peerid MUST be the same as the ntudp_peer_t one
	DBG_ASSERT( local_addr().peerid() == ntudp_peer->local_peerid() );
	// sanity check - the local_addr and remote_addr MUST be fully qualified
	DBG_ASSERT( local_addr().is_fully_qualified() );
	DBG_ASSERT( m_remote_addr.is_fully_qualified() );

	// copy some parameter
	this->ntudp_peer	= ntudp_peer;	
	this->m_remote_addr	= m_remote_addr;

	// if this connection address is already bound, return ntudp_err_t::ALREADY_BOUND
	if( ntudp_peer->cnxaddr_is_bound(local_addr(), remote_addr()) )
		return ntudp_err_t::ALREADY_BOUND;

	// link this object to the ntudp_peer_t
	// - NOTE: it MUST be linked AFTER already_bound check or ntudp_itor_t will conflict with itself
	ntudp_peer->ntudp_itor_link(this);

	// return no error
	return ntudp_err_t::OK;
}



/** \brief Start the operation
 */
ntudp_err_t	ntudp_itor_t::start()		throw()
{
	// sanity check
	DBG_ASSERT( ntudp_peer != NULL );
	DBG_ASSERT( remote_addr().is_null() == false );

	// if no profile is already set, take the one from ntudp_peer_t
	if( !profile )	profile	= nipmem_new ntudp_itor_profile_t(ntudp_peer->get_profile().itor());

	// if the expire_delay has NOT been set, get the default value
	if( expire_delay.is_null() )	expire_delay	= profile->expire_delay();
	// start the timeout
	expire_timeout.start(expire_delay, this, NULL);

	// init some variable for the kad_query_t
	kad_keyid_t	record_keyid	= remote_addr().peerid().to_canonical_string();
	kad_peer_t *	kad_peer	= ntudp_peer->kad_peer();
	kad_err_t	kad_err;
	// create and start the kad_query_t
	kad_query	= nipmem_new kad_query_t();
	kad_err		= kad_query->start(kad_peer, record_keyid, 0, delay_t::ALWAYS, this, NULL);
	if( kad_err.failed() )	return ntudp_err_from_kad(kad_err);
	
	// return no error
	return ntudp_err_t::OK;
}

/** \brief Helper function to start a ntudp_itor_t with the common parameter
 */
ntudp_err_t	ntudp_itor_t::start(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &m_remote_addr
				, ntudp_itor_cb_t *callback, void *userptr)	throw()
{
	ntudp_err_t	ntudp_err;
	// set the callback
	set_callback(callback, userptr);
	// bind the socket
	ntudp_err	= set_remote_addr(ntudp_peer, m_remote_addr);
	if( ntudp_err.failed() )	return ntudp_err;
	// start the operation
	return start();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return ntudp_itor_reverse_t matching of this connection address, or NULL if none is found
 */
ntudp_itor_reverse_t *	ntudp_itor_t::get_reverse_from_nonce(const ntudp_nonce_t &client_nonce)	throw()
{
	std::list<ntudp_itor_reverse_t *>::iterator	iter;
	// go thru all the ntudp_itor_db
	for( iter = reverse_db.begin(); iter != reverse_db.end(); iter++ ){
		ntudp_itor_reverse_t *	itor_reverse	= *iter;
		// if the client_nonce of this ntudp_itor_reverse_t matches, return it
		if( client_nonce == itor_reverse->get_client_nonce() )
			return itor_reverse;
	}
	// return NULL as none has been found.
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_query_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a command in a kad_query_t has an event to notify
 */
bool	ntudp_itor_t::neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << kad_event);
	// sanity check - the kad_event MUST be query_ok()
	DBG_ASSERT( kad_event.is_query_ok() );
	// sanity check - the reporter cb_kad_query MUST be the local one
	DBG_ASSERT( kad_query == &cb_kad_query );
	// sanity check - the kad_event_t MUST be a kad_event_t::RECDUPS
	// - kad_event_t::TIMEDOUT is possible on kad_query but not here as the timeout is delay_t::NEVER
	DBG_ASSERT( kad_event.is_recdups() );

	// copy the kad_recdups_t before deleting the command
	kad_recdups_t	kad_recdups	= kad_event.get_recdups(NULL);

	// delete the kad_query_t and mark is unused
	nipmem_zdelete	kad_query;

	// if no record have been returned, notify a ntudp_event_t::UNKNOWN_HOST
	if( kad_recdups.size() == 0 ){
		std::string reason	= "No Peer Record for " + remote_addr().peerid().to_string();
		notify_callback( ntudp_event_t::build_unknown_host(reason) );
		return false;
	}

	// handle the received kad_recdups_t			
	handle_recved_kad_recdups(kad_recdups);
	
	// log to debug
	KLOG_DBG("direct_db.size()="	<< direct_db.size());
	KLOG_DBG("estarelay_db.size()=" << estarelay_db.size());
	KLOG_DBG("reverse_db.size()="	<< reverse_db.size());

	// if no sub-itor are running, notify a ntudp_event_t::NO_ROUTE_TO_HOST
	if( !sub_itor_is_running() ){
		std::string reason	= "No route to PeerID " + remote_addr().peerid().to_string();
		notify_callback( ntudp_event_t::build_no_route_to_host(reason) );
		return false;
	}

	// return 'dontkeep' as the command has just been deleted
	return false;
}

/** \brief Handle the received kad_recdups
 */
void	ntudp_itor_t::handle_recved_kad_recdups(const kad_recdups_t &kad_recdups)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// go thru all the received kad_rec_t
	for(size_t i = 0; i < kad_recdups.size(); i++){
		const kad_rec_t &	kad_rec	= kad_recdups[i];
		bytearray_t		payload( kad_rec.get_payload() );
		ntudp_peer_rec_t	peer_rec;
		// parse the incoming packet
		try {
			payload	>> peer_rec;
		}catch(serial_except_t &e){
			// log the event
			KLOG_ERR("Can't parse peer record " << kad_rec << " due to " << e.what() );
			// goto the next record even if it is a serious error which MUST NOT happen
			continue;
		}
		// process the incoming peer_rec
		const	ntudp_rdvpt_arr_t &	rdvpt_arr = peer_rec.rdvpt_arr();
		for( size_t i = 0; i < rdvpt_arr.size(); i++ )
			handle_recved_rdvpt(rdvpt_arr[i], peer_rec.npos_res());
		// log to debug
		KLOG_DBG("rdvpt_arr=" << rdvpt_arr);
	}
}

/** \brief Handle the reception of a ntudp_rdvpt_t
 */
void	ntudp_itor_t::handle_recved_rdvpt(const ntudp_rdvpt_t &remote_rdvpt
					, const ntudp_npos_res_t &remote_npos)	throw()
{
	ntudp_cnxtype_t	cnxtype;
	// log to debug
	KLOG_DBG("enter rdvpt=" << remote_rdvpt);

	// if remote_rdvpt is DIRECT, launch a direct_t on it
	// - NOTE: here the cnxtype is not used as it may be the local view of a 
	//   remote peer on the same lan as the local peer.
	//   - and the cnxtype computation is for any peer without taking into
	//     account the specificity of 2 peers on the same LAN.
	if( remote_rdvpt.get_type() == ntudp_rdvpt_t::DIRECT ){
		cnxtype = ntudp_cnxtype_t::DIRECT;
	}else{
		// compute the type of connection possible between both peers
		cnxtype = ntudp_peer->get_npos_res().cpu_cnxtype(remote_npos);
	}
	
	// log to debug
	KLOG_DBG("rdvpt=" << remote_rdvpt << " cnxtype=" << cnxtype);
	
	// launch a sub-itor depending on the cnxtype
	switch( cnxtype.get_value() ){
	case ntudp_cnxtype_t::DIRECT:
			// if another direct is running on the same pserver, do nothing
			if( get_direct_from_pserver(remote_rdvpt.get_addr()) )		break;
			// launch the ntudp_itor_direct_t 
			nipmem_new ntudp_itor_direct_t(this, remote_rdvpt.get_addr(), this, NULL);
			break;
	case ntudp_cnxtype_t::ESTARELAY:{
			// if another estarelay is running on the same pserver, do nothing
			if( get_estarelay_from_pserver(remote_rdvpt.get_addr()))	break;
			// launch the ntudp_itor_estarelay_t
			ntudp_itor_estarelay_t *	itor_estarelay;
			itor_estarelay = nipmem_new ntudp_itor_estarelay_t(this);
			itor_estarelay->start(remote_rdvpt.get_addr(), this, NULL);
			break;}
	case ntudp_cnxtype_t::REVERSE:
			// if another reverse is running on the same pserver, do nothing
			if( get_reverse_from_pserver(remote_rdvpt.get_addr()))		break;
			// launch the ntudp_itor_reverse_t
			nipmem_new ntudp_itor_reverse_t(this, remote_rdvpt.get_addr(), this, NULL);
			break;
	case ntudp_cnxtype_t::IMPOSSIBLE:
			KLOG_ERR("The connection is impossible between the 2 peers.");
			break;
	default:	DBG_ASSERT( 0 );
	}
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          sub-itor callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The common part of all subitor callback
 * 
 * - NOTE: this deletes ALL subitor
 */
void ntudp_itor_t::subitor_common_cb(udp_full_t *udp_full, const pkt_t &estapkt_in
							, const pkt_t &estapkt_out)	throw()
{
	// if no subitor are running (aka they all failed), notify NO_ROUTE_TO_HOST
	if( sub_itor_is_running() == false ){
		std::string reason	= "No route to PeerID " + remote_addr().peerid().to_string();
		notify_callback( ntudp_event_t::build_no_route_to_host(reason) );
		return;
	}

	// if this subitor failed, do nothing
	if( udp_full == NULL )	return;

	// create the ntudp_full_t
	ntudp_full_t *ntudp_full = nipmem_new ntudp_full_t(ntudp_peer, local_addr(), remote_addr(), udp_full);
	// set the ntudp_full_t estapkt
	ntudp_full->set_estapkt(estapkt_in, estapkt_out);

	// delete all the subitor
	delete_all_subitor();

	// notify the event
	ntudp_event_t	ntudp_event	= ntudp_event_t::build_cnx_established(ntudp_full);
	notify_callback( ntudp_event );
}

/** \brief callback notified by a ntudp_itor_direct_t has a result to notify
 */
bool ntudp_itor_t::neoip_ntudp_itor_direct_cb(void *cb_userptr, ntudp_itor_direct_t &cb_itor_direct
		, udp_full_t *udp_full, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	ntudp_itor_direct_t *	itor_direct	= &cb_itor_direct;
	// if this subitor failed, delete it
	if( udp_full == NULL )	nipmem_delete itor_direct;
	// call the subitor_common_cb
	subitor_common_cb(udp_full, estapkt_in, estapkt_out);
	// return false - as the subitor is mandatorily deleted
	return false;
}

/** \brief callback notified by a ntudp_itor_estarelay_t has a result to notify
 */
bool ntudp_itor_t::neoip_ntudp_itor_estarelay_cb(void *cb_userptr, ntudp_itor_estarelay_t &cb_itor_estarelay
		, udp_full_t *udp_full, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	ntudp_itor_estarelay_t*	itor_estarelay	= &cb_itor_estarelay;
	// if this subitor failed, delete it
	if( udp_full == NULL )	nipmem_delete itor_estarelay;
	// call the subitor_common_cb
	subitor_common_cb(udp_full, estapkt_in, estapkt_out);
	// return false - as the subitor is mandatorily deleted
	return false;
}

/** \brief callback notified by a ntudp_itor_reverse_t has a result to notify
 */
bool ntudp_itor_t::neoip_ntudp_itor_reverse_cb(void *cb_userptr, ntudp_itor_reverse_t &cb_itor_reverse
		, udp_full_t *udp_full, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	ntudp_itor_reverse_t *	itor_reverse	= &cb_itor_reverse;
	// if this subitor failed, delete it
	if( udp_full == NULL )	nipmem_delete itor_reverse;
	// call the subitor_common_cb
	subitor_common_cb(udp_full, estapkt_in, estapkt_out);
	// return false - as the subitor is mandatorily deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Utility function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if some 'sub-itor' are currently running, false otherwise
 */
bool	ntudp_itor_t::sub_itor_is_running()	const throw()
{
	// if any 'sub-itor' database is not empty, return true
	if( !direct_db.empty() )	return true;
	if( !estarelay_db.empty() )	return true;
	if( !reverse_db.empty() )	return true;
	// if all 'sub-itor' database are empty, return false
	return false;
}

/** \brief Delete all the pending subitor
 */
void	ntudp_itor_t::delete_all_subitor()	throw()
{
	// close all pending ntudp_itor_direct_t
	while( !direct_db.empty() )	nipmem_delete	direct_db.front();
	// close all pending ntudp_itor_estarelay_t
	while( !estarelay_db.empty() )	nipmem_delete	estarelay_db.front();
	// close all pending ntudp_itor_reverse_t
	while( !reverse_db.empty() )	nipmem_delete	reverse_db.front();
}

/** \brief Return the ntudp_itor_direct_t which uses the pserver_ipport, or NULL if none does
 */
ntudp_itor_direct_t *	ntudp_itor_t::get_direct_from_pserver(const ipport_addr_t &pserver_ipport) throw()
{
	std::list<ntudp_itor_direct_t *>::iterator	iter;
	// go thru the whole direct_db
	for( iter = direct_db.begin(); iter != direct_db.end(); iter++ ){
		ntudp_itor_direct_t *	itor_direct = *iter;
		// if this ntudp_itor_direct_t doesnt use the same pserver_ipport, skip it
		if( pserver_ipport != itor_direct->get_pserver_ipport() )	continue;
		// if it does use it, return the pointer
		return itor_direct;
	}
	// if this point is reached, none match, so return NULL
	return NULL;
}

/** \brief Return the ntudp_itor_estarelay_t which uses the pserver_ipport, or NULL if none does
 */
ntudp_itor_estarelay_t *	ntudp_itor_t::get_estarelay_from_pserver(const ipport_addr_t &pserver_ipport) throw()
{
	std::list<ntudp_itor_estarelay_t *>::iterator	iter;
	// go thru the whole estarelay_db
	for( iter = estarelay_db.begin(); iter != estarelay_db.end(); iter++ ){
		ntudp_itor_estarelay_t *	itor_estarelay = *iter;
		// if this ntudp_itor_estarelay_t doesnt use the same pserver_ipport, skip it
		if( pserver_ipport != itor_estarelay->get_pserver_ipport() )	continue;
		// if it does use it, return the pointer
		return itor_estarelay;
	}
	// if this point is reached, none match, so return NULL
	return NULL;
}

/** \brief Return the ntudp_itor_reverse_t which uses the pserver_ipport, or NULL if none does
 */
ntudp_itor_reverse_t *	ntudp_itor_t::get_reverse_from_pserver(const ipport_addr_t &pserver_ipport) throw()
{
	std::list<ntudp_itor_reverse_t *>::iterator	iter;
	// go thru the whole reverse_db
	for( iter = reverse_db.begin(); iter != reverse_db.end(); iter++ ){
		ntudp_itor_reverse_t *	itor_reverse = *iter;
		// if this ntudp_itor_reverse_t doesnt use the same pserver_ipport, skip it
		if( pserver_ipport != itor_reverse->get_pserver_ipport() )	continue;
		// if it does use it, return the pointer
		return itor_reverse;
	}
	// if this point is reached, none match, so return NULL
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the expire_timeout expire
 */
bool ntudp_itor_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// notify a CNX_ESTA_TIMEDOUT to the caller	
	std::string reason	= "Connection establishment timedout";
	return notify_callback( ntudp_event_t::build_cnx_esta_timedout(reason) );	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      to_string() function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	ntudp_itor_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return remote_addr().to_string();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	ntudp_itor_t::notify_callback(const ntudp_event_t &ntudp_event)	throw()
{
	// sanity check - no sub-itor are supposed to run when a event is notified
	DBG_ASSERT( !sub_itor_is_running() );
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_itor_event_cb( userptr, *this, ntudp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - specific to ntudp_itor_t, the object MUST be deleted during notification
	DBG_ASSERT( tokeep == false );	
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END




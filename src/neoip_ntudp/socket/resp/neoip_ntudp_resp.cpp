/*! \file
    \brief Definition of the ntudp_resp_t
    
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_resp.hpp"
#include "neoip_ntudp_resp_direct.hpp"
#include "neoip_ntudp_resp_estarelay.hpp"
#include "neoip_ntudp_resp_reverse.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_sock_pkttype.hpp"
#include "neoip_ntudp_sock_errcode.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_resp_t::ntudp_resp_t()		throw()
{
	// zero some fields
	ntudp_peer	= NULL;
	callback	= NULL;
	profile		= NULL;
}

/** \brief destructor
 */
ntudp_resp_t::~ntudp_resp_t()		throw()
{
	// delete the profile if needed
	if( profile )			nipmem_delete	profile;
	// unlink this object from the ntudp_peer_t
	if( ntudp_peer )		ntudp_peer->ntudp_resp_unlink(this);
	// close all pending ntudp_resp_direct_t
	while( !direct_db.empty() )	nipmem_delete	direct_db.front();
	// close all pending ntudp_resp_estarelay_t
	while( !estarelay_db.empty() )	nipmem_delete	estarelay_db.front();
	// close all pending ntudp_resp_reverse_t
	while( !reverse_db.empty() )	nipmem_delete	reverse_db.front();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      Setup Function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_resp_t &	ntudp_resp_t::set_profile(const ntudp_resp_profile_t &profile)throw()
{
	// sanity check - the local profile MUST NOT be already set
	DBG_ASSERT( this->profile == NULL );
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );
	// copy the parameter
	this->profile	= nipmem_new ntudp_resp_profile_t(profile);
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
ntudp_err_t	ntudp_resp_t::start(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &param_listen_addr
					, ntudp_resp_cb_t *callback, void *userptr)	throw()
{
	// copy some parameter
	this->ntudp_peer	= ntudp_peer;
	this->m_listen_addr	= param_listen_addr;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// if the listen_addr is null, set it to a random portid
	if( m_listen_addr.is_null() )
		m_listen_addr = ntudp_addr_t(ntudp_peerid_t(),ntudp_portid_t::build_random());
	// if the listen_addr peerid is null, set it with the ntudp_peer one
	if( m_listen_addr.peerid().is_null() )	m_listen_addr.peerid( ntudp_peer->local_peerid() );
	// sanity check - the listen_addr MUST be fully qualified
	DBG_ASSERT( listen_addr().is_fully_qualified() );
	// sanity check - the listen_addr peerid MUST be the ntudp_peer local peerid
	DBG_ASSERT( listen_addr().peerid() == ntudp_peer->local_peerid() );
	
	// if the portid is already bound, return ntudp_err_t::ALREADY_BOUND
	if( ntudp_peer->resp_from_portid(listen_addr().portid()) )
		return ntudp_err_t::ALREADY_BOUND;

	// if no profile is already set, take the one from ntudp_peer_t
	if( !profile )	profile	= nipmem_new ntudp_resp_profile_t(ntudp_peer->get_profile().resp());

	// link this object to the ntudp_peer_t
	ntudp_peer->ntudp_resp_link(this);
	
	// return no error
	return ntudp_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   cnx_request notification from relpeer_t or dircnx_srv_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief For the caller to notify a recved DIRECT_CNX_REQUEST
 * 
 * - NOTE: this function DOESNT return a 'tokeep' as it is garanteed not to notify anything
 *         in this iteration of the event loop.
 */
void	ntudp_resp_t::notify_recved_direct_cnx_request(pkt_t &orig_pkt, udp_full_t *udp_full)throw()
{
	// create a ntudp_resp_direct_t - it autolink itself to the ntudp_resp_t
	nipmem_new ntudp_resp_direct_t(this, orig_pkt, udp_full, this, NULL);
}

/** \brief static function called to notify just recved datum_t in ntudp_tunl_pkttype_t::PKT_SRV2CLI
 * 
 * - WARNING: it is *NOT* allowed to notify the caller of the ntudp layer inside function
 * - this is a static function as it is addressed to ntudp_resp_t class itself without specific
 *   instance
 * 
 * @return a datum_t of the packet to reply. if .is_null(), dont reply anything
 */
datum_t	ntudp_resp_t::notify_recved_pkt_tunl_srv2cli(ntudp_peer_t *ntudp_peer
						, const datum_t &recved_datum)	throw()
{
	ntudp_resp_estarelay_t *resp_estarelay;
	ntudp_resp_reverse_t *	resp_reverse;
	ntudp_sock_pkttype_t	pkttype;
	ntudp_nonce_t		incoming_nonce;
	ntudp_peerid_t		dst_peerid;
	ntudp_portid_t		dst_portid;	
	ipport_addr_t		remote_ipport;
	pkt_t			pkt(recved_datum);
	// parse the incoming packet
	try {
		pkt >> pkttype;
		pkt >> incoming_nonce;
		pkt >> dst_peerid;
		pkt >> dst_portid;
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse incoming packet due to " << e.what() );
		return datum_t();
	}
	
	// if the destination peerid is not the local one, discard this packet
	if( dst_peerid != ntudp_peer->local_peerid() )	return datum_t();
	
	// try to find the ntudp_resp_t matching the destination portid
	ntudp_resp_t *	ntudp_resp	= ntudp_peer->resp_from_portid(dst_portid);
	// if no ntudp_resp_t matches, return a ntudp_sock_errcode_t::UNREACH_PORTID
	if( !ntudp_resp ){
		pkt_t	pkt_err	= ntudp_sock_errcode_build_pkt(ntudp_sock_errcode_t::UNREACH_PORTID
								, incoming_nonce);
		return pkt_err.to_datum();
	}

	// handle the packet according to the pkttype
	switch( pkttype.get_value() ){
	case ntudp_sock_pkttype_t::ESTARELAY_CNX_REQUEST:
			resp_estarelay	= nipmem_new ntudp_resp_estarelay_t(ntudp_resp, ntudp_resp, NULL);
			return resp_estarelay->start(recved_datum);
	case ntudp_sock_pkttype_t::REVERSE_CNX_REQUEST:
			resp_reverse	= nipmem_new ntudp_resp_reverse_t(ntudp_resp, ntudp_resp, NULL);
			return resp_reverse->start(recved_datum);
	default:	// any other pkttype are ignored
			return datum_t();
	}
	return datum_t();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          sub-resp callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief The common part of all subresp callback, the one which notify the caller of a sucess
 * 
 * @return false if the ntudp_resp_t has been deleted, true otherwise
 */
bool ntudp_resp_t::notify_cnx_established(const ntudp_addr_t &local_addr, const ntudp_addr_t &remote_addr
		, udp_full_t *udp_full, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	// sanity check - the udp_full_t MUST be non null
	DBG_ASSERT( udp_full );

	// create the ntudp_full_t
	ntudp_full_t *ntudp_full = nipmem_new ntudp_full_t(ntudp_peer, local_addr, remote_addr, udp_full);
	// set the ntudp_full_t estapkt
	ntudp_full->set_estapkt(estapkt_in, estapkt_out);

	// notify the event
	ntudp_event_t	ntudp_event	= ntudp_event_t::build_cnx_established(ntudp_full);
	return notify_callback( ntudp_event );
}

/** \brief callback notified by a ntudp_resp_direct_t has a result to notify
 */
bool ntudp_resp_t::neoip_ntudp_resp_direct_cb(void *cb_userptr, ntudp_resp_direct_t &cb_resp_direct
		, udp_full_t *udp_full, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	ntudp_resp_direct_t *	resp_direct = &cb_resp_direct;
	// if the subresp failed, just delete the subresp
	if( udp_full == NULL ){
		nipmem_delete resp_direct;
		return false;
	}

	// if this sub-resp succeed, notify the caller that a connection is established
	bool 	tokeep	= notify_cnx_established(resp_direct->local_addr(), resp_direct->remote_addr()
							, udp_full, estapkt_in, estapkt_out);
	if( !tokeep )	return false;

	// delete all the subresp for this connection address
	// - thus if there are other subresp for this cnxaddr, they will be deleted
	delete_subresp_for_cnxaddr(resp_direct->local_addr(), resp_direct->remote_addr());
	// return dontkeep
	return false;	
}

/** \brief callback notified by a ntudp_resp_estarelay_t has a result to notify
 */
bool ntudp_resp_t::neoip_ntudp_resp_estarelay_cb(void *cb_userptr, ntudp_resp_estarelay_t &cb_resp_estarelay
		, udp_full_t *udp_full, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	ntudp_resp_estarelay_t *	resp_estarelay = &cb_resp_estarelay;
	// if the subresp failed, just delete the subresp
	if( udp_full == NULL ){
		nipmem_delete resp_estarelay;
		return false;
	}

	// if this sub-resp succeed, notify the caller that a connection is established
	bool 	tokeep	= notify_cnx_established(resp_estarelay->local_addr(), resp_estarelay->remote_addr()
						, udp_full, estapkt_in, estapkt_out);
	if( !tokeep )	return false;

	// delete all the subresp for this connection address
	// - thus if there are other subresp for this cnxaddr, they will be deleted
	delete_subresp_for_cnxaddr(resp_estarelay->local_addr(), resp_estarelay->remote_addr());
	// return dontkeep
	return false;	
}

/** \brief callback notified by a ntudp_resp_reverse_t has a result to notify
 */
bool ntudp_resp_t::neoip_ntudp_resp_reverse_cb(void *cb_userptr, ntudp_resp_reverse_t &cb_resp_reverse
		, udp_full_t *udp_full, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw()
{
	ntudp_resp_reverse_t *	resp_reverse = &cb_resp_reverse;
	// if the subresp failed, just delete the subresp
	if( udp_full == NULL ){
		nipmem_delete resp_reverse;
		return false;
	}

	// if this sub-resp succeed, notify the caller that a connection is established
	bool 	tokeep	= notify_cnx_established(resp_reverse->local_addr(), resp_reverse->remote_addr()
						, udp_full, estapkt_in, estapkt_out);
	if( !tokeep )	return false;

	// delete all the subresp for this connection address
	// - thus if there are other subresp for this cnxaddr, they will be deleted
	delete_subresp_for_cnxaddr(resp_reverse->local_addr(), resp_reverse->remote_addr());
	// return dontkeep
	return false;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 Utility function to handle subresp
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Delete all the subresp matching this connection address
 * 
 * - it is used to delete all the subresp of a given connection addresses once a subresp
 *   succeed the establishment
 * - some funkyness as it is needed to go thru a list while deleting its elements
 */
void ntudp_resp_t::delete_subresp_for_cnxaddr(const ntudp_addr_t &orig_local_addr
						, const ntudp_addr_t &orig_remote_addr) throw()
{
	// copy the parameter in case they comes from one of the subresp that gonna be deleted
	ntudp_addr_t	local_addr	= orig_local_addr;
	ntudp_addr_t	remote_addr	= orig_remote_addr;
	// delete all the ntudp_resp_direct_t matching this connection
	ntudp_resp_direct_t *resp_direct;
	do {
		resp_direct	= get_resp_direct_for_cnxaddr(local_addr,remote_addr);
		if( resp_direct )	nipmem_delete resp_direct;
	} while( resp_direct );

	// delete all the ntudp_resp_estarelay_t matching this connection
	ntudp_resp_estarelay_t *resp_estarelay;
	do {
		resp_estarelay	= get_resp_estarelay_for_cnxaddr(local_addr,remote_addr);
		if( resp_estarelay )	nipmem_delete resp_estarelay;
	} while( resp_estarelay );

	// delete all the ntudp_resp_reverse_t matching this connection
	ntudp_resp_reverse_t *resp_reverse;
	do {
		resp_reverse	= get_resp_reverse_for_cnxaddr(local_addr,remote_addr);
		if( resp_reverse )	nipmem_delete resp_reverse;
	} while( resp_reverse );
}
/** \brief Get the first ntudp_resp_direct_t which match this connection address
 */
ntudp_resp_direct_t * ntudp_resp_t::get_resp_direct_for_cnxaddr(const ntudp_addr_t &local_addr
						, const ntudp_addr_t &remote_addr) throw()
{
	std::list<ntudp_resp_direct_t *>::iterator	iter;
	// go thru the whole direct_db
	for( iter = direct_db.begin(); iter != direct_db.end(); iter++ ){
		ntudp_resp_direct_t *	resp_direct	= *iter;
		// if this resp_direct local address doesnt match, skip it
		if( local_addr  != resp_direct->local_addr() )	continue;
		// if this resp_direct remote address doesnt match, skip it
		if( remote_addr != resp_direct->remote_addr() )	continue;
		// this resp_direct match, so return it
		return resp_direct;
	}
	// if this point is reached, none has been found so return NULL;
	return NULL;
}


/** \brief Get the first ntudp_resp_estarelay_t which match this connection address
 */
ntudp_resp_estarelay_t * ntudp_resp_t::get_resp_estarelay_for_cnxaddr(const ntudp_addr_t &local_addr
						, const ntudp_addr_t &remote_addr) throw()
{
	std::list<ntudp_resp_estarelay_t *>::iterator	iter;
	// go thru the whole estarelay_db
	for( iter = estarelay_db.begin(); iter != estarelay_db.end(); iter++ ){
		ntudp_resp_estarelay_t *	resp_estarelay	= *iter;
		// if this resp_estarelay local address doesnt match, skip it
		if( local_addr  != resp_estarelay->local_addr() )	continue;
		// if this resp_estarelay remote address doesnt match, skip it
		if( remote_addr != resp_estarelay->remote_addr() )	continue;
		// this resp_estarelay match, so return it
		return resp_estarelay;
	}
	// if this point is reached, none has been found so return NULL;
	return NULL;
}

/** \brief Get the first ntudp_resp_reverse_t which match this connection address
 */
ntudp_resp_reverse_t * ntudp_resp_t::get_resp_reverse_for_cnxaddr(const ntudp_addr_t &local_addr
						, const ntudp_addr_t &remote_addr) throw()
{
	std::list<ntudp_resp_reverse_t *>::iterator	iter;
	// go thru the whole reverse_db
	for( iter = reverse_db.begin(); iter != reverse_db.end(); iter++ ){
		ntudp_resp_reverse_t *	resp_reverse	= *iter;
		// if this resp_reverse local address doesnt match, skip it
		if( local_addr  != resp_reverse->local_addr() )	continue;
		// if this resp_reverse remote address doesnt match, skip it
		if( remote_addr != resp_reverse->remote_addr() )	continue;
		// this resp_reverse match, so return it
		return resp_reverse;
	}
	// if this point is reached, none has been found so return NULL;
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      to_string() function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	ntudp_resp_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return listen_addr().to_string();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	ntudp_resp_t::notify_callback(const ntudp_event_t &ntudp_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_resp_event_cb( userptr, *this, ntudp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END




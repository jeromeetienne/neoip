/*! \file
    \brief Definition of the ntudp_relpeer_t

\par Brief Description
\ref ntudp_relpeer_t maintains a pool of tunnels toward \ref ntudp_pserver_t
- It supports to have several of them to increase robustness when a ntudp_pserver_t
  goes offline ungracefully.
- Each tunnel is handled by \ref ntudp_relpeer_tunnel_t.

*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_ntudp_relpeer.hpp"
#include "neoip_ntudp_relpeer_wikidbg.hpp"
#include "neoip_ntudp_relpeer_tunnel.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
#include "neoip_ntudp_dircnx_server.hpp"
#include "neoip_ntudp_peer.hpp"
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
ntudp_relpeer_t::ntudp_relpeer_t()				throw()
{
	// zero some field
	ntudp_peer	= NULL;
	dircnx_server	= NULL;	
}

/** \brief destructor
 */
ntudp_relpeer_t::~ntudp_relpeer_t()				throw()
{
	// delete the dircnx_server if needed
	nipmem_zdelete	dircnx_server;	
	// unsubscribe to the pserver_pool_t if needed
	if( ntudp_peer ){
		ntudp_pserver_pool_t *	pserver_pool = ntudp_peer->pserver_pool();	
		pserver_pool->new_unknown_unsubscribe(this, NULL);
	}
	// free all tunnel_db
	while( !tunnel_db.empty() ){
		std::set<ntudp_relpeer_tunnel_t *>::iterator	iter = tunnel_db.begin();
		// delete the object itself
		nipmem_delete	*iter;
		// remove it from the tunnel_db
		tunnel_db.erase(iter);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Setup Function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
ntudp_relpeer_t &	ntudp_relpeer_t::set_profile(const ntudp_relpeer_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == ntudp_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
ntudp_err_t	ntudp_relpeer_t::start(ntudp_peer_t *ntudp_peer, ntudp_relpeer_cb_t *callback
							, void *userptr)	throw()
{	
	ntudp_err_t	ntudp_err;
	// copy the parameter
	this->ntudp_peer	= ntudp_peer;
	this->callback		= callback;
	this->userptr		= userptr;
	this->nb_needed_tunnel	= profile.nb_needed_tunnel();
	// subscribe to the pserver_pool_t
	ntudp_pserver_pool_t *	pserver_pool = ntudp_peer->pserver_pool();	
	pserver_pool->new_unknown_subscribe(this, NULL);

	// start the dircnx_server to receive direct connection
	dircnx_server	= nipmem_new ntudp_dircnx_server_t();
	ntudp_err	= dircnx_server->start(ntudp_peer);	
	if( ntudp_err.failed() )	return ntudp_err;
	
	// create all the needed ntudp_relpeer_tunnel_t
	create_needed_tunnel();		

	// return no error
	return ntudp_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the rdvpt representing all the established tunnels
 */
ntudp_rdvpt_arr_t	ntudp_relpeer_t::get_established_tunnel_rdvpt()	const throw()
{
	std::set<ntudp_relpeer_tunnel_t *>::const_iterator	iter;
	ntudp_rdvpt_arr_t					rdvpt_arr;
	// scan all the tunnel in tunnel_db
	for( iter = tunnel_db.begin(); iter != tunnel_db.end(); iter++ ){
		const ntudp_relpeer_tunnel_t *tunnel	= *iter;
		// if this tunnel is not established, skip it
		if( tunnel->is_established() == false )	continue;
		// if the tunnel is established, add it to the rdvpt_arr
		rdvpt_arr	+= ntudp_rdvpt_t(tunnel->get_pserver_addr(), ntudp_rdvpt_t::RELAY);
	}
	// return the build ntudp_rdvpt_arr_t
	return rdvpt_arr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           tunnel creation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if a tunnel uses the pserver_addr as remote address, false otherwise
 */
bool	ntudp_relpeer_t::tunnel_use_pserver_addr(const ipport_addr_t &pserver_addr)	const throw()
{
	std::set<ntudp_relpeer_tunnel_t *>::const_iterator	iter;
	// scan all the tunnel in tunnel_db
	for( iter = tunnel_db.begin(); iter != tunnel_db.end(); iter++ ){
		const ntudp_relpeer_tunnel_t *tunnel	= *iter;
		// if this tunnel has this pserver_addr, return true now
		if( pserver_addr == tunnel->get_pserver_addr() )	return true;
	}
	// if this point is reached, no tunnel uses this remote address
	return false;
}


/** \brief Try to create all the needed tunnel
 */
void	ntudp_relpeer_t::create_needed_tunnel()	throw()
{
	// sanity check - the current number of tunnel MUST be <= than the nb_needed_tunnel
	DBG_ASSERT( nb_needed_tunnel >= tunnel_db.size() );
	// compute the number of tunnel to create
	size_t	nb_to_create	= nb_needed_tunnel - tunnel_db.size();
	//  start all the initial discovery
	for(size_t i = 0; i < nb_to_create; i++)
		create_one_tunnel();
}

/** \brief Create a new ntudp_relpeer_tunnel_t
 * 
 * @return true on error, false otherwise
 */
bool	ntudp_relpeer_t::create_one_tunnel()	throw()
{
	ntudp_pserver_pool_t *	pserver_pool = ntudp_peer->pserver_pool();
	ipport_addr_t		pserver_addr;
	ntudp_err_t		ntudp_err;

	// log to debug
	KLOG_DBG("enter");

	// try to find a useable address in the pserver_pool->reach_db
	// - with usable meaning 'not already used by another tunnel
	for(size_t i = 0; i < pserver_pool->reach_size() && pserver_addr.is_null(); i++){
		// if this one is already used by a tunnel, skip it
		if( tunnel_use_pserver_addr(pserver_pool->reach_at(i)) )	continue;
		// else it is free to use, so copy it pserver_addr and leave the loop
		pserver_addr	 = pserver_pool->reach_at(i);
	}

	// try to find a useable address in the pserver_pool->unknown_db	
	// - with usable meaning 'not already used by another tunnel
	for(size_t i = 0; i < pserver_pool->unknown_size() && pserver_addr.is_null(); i++){
		// if this one is already used by a tunnel, skip it
		if( tunnel_use_pserver_addr(pserver_pool->unknown_at(i)) )	continue;
		// else it is free to use, so copy it pserver_addr and leave the loop
		pserver_addr	 = pserver_pool->unknown_at(i);
	}

	// if no usable pserver_addr has been found, ask for more to the pserver_pool_t and exit
	if( pserver_addr.is_null() ){
		pserver_pool->get_more_unknown();
		return true;
	}

	// create a ntudp_relpeer_tunnel_t toward the pserver_addr
	ntudp_relpeer_tunnel_t *tunnel;
	tunnel		= nipmem_new ntudp_relpeer_tunnel_t();
	ntudp_err	= tunnel->set_profile(profile.tunnel()).start(pserver_addr, ntudp_peer, this, NULL);
	if( ntudp_err.failed() ){
		nipmem_zdelete tunnel;
		return true;
	}

	// put it in the database
	bool	succeed	= tunnel_db.insert(tunnel).second;
	DBG_ASSERT( succeed );

	// return no error
	return false;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ntudp_pserver_pool_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a event_hook_t level is notified
 * 
 * @return a 'tokeep' for the event_hook_t object
 */
bool	ntudp_relpeer_t::neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// create all the needed tunnel
	create_needed_tunnel();	
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ntudp_relpeer_tunnel_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_relpeer_tunnel_t when to notify an event
 */
bool	ntudp_relpeer_t::neoip_ntudp_relpeer_tunnel_cb(void *cb_userptr, ntudp_relpeer_tunnel_t &cb_tunnel
							, const bool established)	throw()
{
	ntudp_relpeer_tunnel_t *tunnel = &cb_tunnel;
	// log to debug
	KLOG_ERR("enter");	
	// if the tunnel is not/nomore established, delete it and replace it
	if( !established ){
		// delete the object and remove it from the database
		tunnel_db.erase(tunnel);
		nipmem_zdelete tunnel;
		// recreate the needed tunnels
		create_needed_tunnel();
	}

	// notify the callback of change in the established tunnel
	notify_callback();

	// if the tunnel has been deleted, return 'dontkeep'
	if( !established )	return false;
	// return 'tokeep'
	return true;
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
bool 	ntudp_relpeer_t::notify_callback()	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_relpeer_cb(userptr, *this);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END




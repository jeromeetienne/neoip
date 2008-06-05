/*! \file
    \brief Class to handle the bt_session_t
    
*/

/* system include */
/* local include */
#include "neoip_bt_session.hpp"
#include "neoip_bt_session_cnx.hpp"
#include "neoip_bt_session_helper.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_itor_arg.hpp"
#include "neoip_socket_resp_arg.hpp"
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
bt_session_t::bt_session_t()		throw()
{
	// zero some field
	socket_resp	= NULL;
	is_tcp_inetreach= false;
}

/** \brief Destructor
 */
bt_session_t::~bt_session_t()		throw()
{
	// close all pending bt_swarm_t
	DBGNET_ASSERT( swarm_db.empty() );
	while( !swarm_db.empty() )	nipmem_delete swarm_db.front();	
	// close all pending bt_session_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();	
	// delete the socket_resp if needed
	nipmem_zdelete	socket_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_session_t &	bt_session_t::profile(const bt_session_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}
/** \brief Start the operation
 */
bt_err_t bt_session_t::start(const ipport_aview_t &p_listen_aview, const bt_id_t &p_local_peerid)	throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_ERR("p_listen_aview=" << p_listen_aview);
	// copy the parameter
	this->m_listen_aview	= p_listen_aview;
	
	// start the socket_resp_t
	socket_resp_arg_t	resp_arg;
	resp_arg	= bt_session_helper_t::build_socket_resp_arg(listen_lview(), this);
	socket_resp	= nipmem_new socket_resp_t();
	socket_err	= socket_resp->start(resp_arg, this, NULL);
	if( socket_err.failed() )	return bt_err_from_socket(socket_err);

	// update the listen_aview in case socket_resp_t did dynamic binding
	ipport_addr_t	bound_listen_lview = socket_resp->listen_addr().get_peerid_vapi()->to_string()
					+ std::string(":")
					+ socket_resp->listen_addr().get_portid_vapi()->to_string();
	this->m_listen_aview.update_once_bound(bound_listen_lview);

	// set the local_peerid - if the parameter is unset, get 
	if( !p_local_peerid.is_null() )	m_local_peerid	= p_local_peerid;
	else				m_local_peerid	= bt_id_t::build_random();

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/bt_session_" + local_peerid().to_string());
	// return no error
	return bt_err_t::OK;
}

/** \brief return a pointer on a bt_swarm_t matching this infohash, or NULL if none does
 */
bt_swarm_t *	bt_session_t::swarm_by_infohash(const bt_id_t &infohash)	throw()
{
	std::list<bt_swarm_t *>::iterator	iter;
	// go thru the whole swarm_db
	for(iter = swarm_db.begin(); iter != swarm_db.end(); iter++){
		bt_swarm_t *bt_swarm	= *iter;
		// if the infohash of this bt_swarm_t doesnt match, skip it
		if( bt_swarm->get_mfile().infohash() != infohash )	continue;
		// return this bt_swarm
		return bt_swarm;
	}
	// if this point is reached, no bt_swarm_t has been found
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_resp_t when a connection is established
 */
bool	bt_session_t::neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( socket_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:{
			bt_session_cnx_t *	session_cnx;
			bt_err_t		bt_err;
			session_cnx	= nipmem_new bt_session_cnx_t();
			bt_err		= session_cnx->start(this, socket_event.get_cnx_established());
			if( bt_err.failed() )	nipmem_delete session_cnx;
			return true;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Function used to update the listen_pview *DURING* bt_session_t run
 * 
 * - this function updates the linked bt_swarm_t too
 */
void	bt_session_t::update_listen_pview(const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter NOT YET IMPLEMENTED new_listen_pview=" << new_listen_pview);
	
	// update the m_listen_aview
	m_listen_aview.pview	(new_listen_pview);

	// go thru the whole swarm_db
	std::list<bt_swarm_t *>::iterator	iter;
	for(iter = swarm_db.begin(); iter != swarm_db.end(); iter++){
		bt_swarm_t *bt_swarm	= *iter;
		// update this bt_swarm_t listen_pview
		bt_swarm->update_listen_pview(new_listen_pview);
	}	
}

NEOIP_NAMESPACE_END






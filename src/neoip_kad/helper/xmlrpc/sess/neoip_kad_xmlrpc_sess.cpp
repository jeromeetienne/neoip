/*! \file
    \brief Definition of the \ref kad_xmlrpc_t::xmlrpc_sess_t class

\par Brief Description
This implement the xmlrpc session for the NeoiIP Kademlia.
It is needed as the xmlrpc are stateless, so the session MUST timeout 
after some time of inactivity as a self-healing process if the xmlrpc
client did an unclean exit, aka without stopping the session.

\par TODO
- TODO make the timeout tunable

*/

/* system include */
/* local include */
#include "neoip_kad_xmlrpc_sess.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_xmlrpc_t::xmlrpc_sess_t::xmlrpc_sess_t(kad_xmlrpc_t *kad_xmlrpc)	throw()
{
	const kad_xmlrpc_profile_t &profile	= kad_xmlrpc->profile;
	// log to debug
	KLOG_ERR("enter");
	// copy some parameter
	this->kad_xmlrpc	= kad_xmlrpc;
	this->peer_slotid	= slotpool_t::NONE;
	// allocate the session slot_id
	sess_slotid		= kad_xmlrpc->sess_slotpool.allocate(this);
	// link itself to kad_xmlrpc_t
	kad_xmlrpc->sess_link(this);
	
	// start the idle_timeout
	idle_timeout.start(profile.session_idle_timeout(), this, NULL);
}

/** \brief Destructor
 */
kad_xmlrpc_t::xmlrpc_sess_t::~xmlrpc_sess_t()			throw()
{
	// log to debug
	KLOG_ERR("enter");
	// unsubscribe the realm if needed
	if( peer_slotid != slotpool_t::NONE )	get_kad_listener().peer_unsubscribe(peer_slotid);
	// unlink itself from kad_xmlrpc_t
	kad_xmlrpc->sess_unlink(this);
	// release the session slot_id
	kad_xmlrpc->sess_slotpool.release(sess_slotid);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           Start the operation
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief start the operation
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_sess_t::start(const kad_realmid_t &realmid
					, const kad_peerid_t &peerid
					, const kad_profile_t &profile)		throw()
{
	kad_err_t	kad_err;
	// subcribe to the realm
	kad_err	= get_kad_listener().peer_subscribe(realmid, peerid, profile, &peer_slotid);
	if( kad_err.failed() )	return kad_err;
	// return no error
	return kad_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         idle_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	kad_xmlrpc_t::xmlrpc_sess_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)
										throw()
{
	// autodelete
	nipmem_delete	this;
	// return 'dontkeep'
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      notify_new_command
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief to notify that a command has been received on this session
 * 
 * - this function just refresh the idle timeout
 */
void	kad_xmlrpc_t::xmlrpc_sess_t::notify_new_command()	throw()
{
	const kad_xmlrpc_profile_t &profile	= kad_xmlrpc->profile;
	// reinit the idle_timeout
	idle_timeout.start(profile.session_idle_timeout(), this, NULL);
}


NEOIP_NAMESPACE_END






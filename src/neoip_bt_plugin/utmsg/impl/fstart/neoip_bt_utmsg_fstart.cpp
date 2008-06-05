/*! \file
    \brief Class to handle the bt_utmsg_fstart_t

\par Brief Description
\rev bt_utmsg_fstart_t is an extension which keeps a state to determine
if a connection is in 'faststart' or not. If in faststart, the bt_peerpick_vapi_t
may promote it more easily to allow a download. And thus the faststart connection
will be able to download faster and reduce the establishement latency when
just joining a swarm.
- WARNING: this is a 'full-trust' mechanism. aka it assumes that the remote 
  peer is honnest.
  - this could be easily abused by an attacker which would declare itself permanently
    in 'faststart' and thus get elected by bt_peerpick_vapi_t more frequently
    than honnest peers.
    
*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_fstart.hpp"
#include "neoip_bt_utmsg_fstart_cnx.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_utmsg_event.hpp"
#include "neoip_bt_cmd.hpp"
#include "neoip_bencode.hpp"
#include "neoip_dvar.hpp"
#include "neoip_pkt.hpp"
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
bt_utmsg_fstart_t::bt_utmsg_fstart_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	utmsg_cb	= NULL;
	m_local_fstart	= false;
}

/** \brief Destructor
 */
bt_utmsg_fstart_t::~bt_utmsg_fstart_t()		throw()
{
	// unregister this object
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( utmsg_cb ){
		bool	tokeep	= notify_utmsg_cb( bt_utmsg_event_t::build_unregister() );
		DBG_ASSERT( tokeep == true );
	}
	// close all pending bt_utmsg_fstart_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_utmsg_fstart_t::start(bt_swarm_t *bt_swarm, bt_utmsg_cb_t *utmsg_cb
							, void *userptr)	throw()
{
	bool	tokeep;
	// copy the parameter
	this->bt_swarm	= bt_swarm;
	this->utmsg_cb	= utmsg_cb;
	this->userptr	= userptr;
	// register this utmsg to the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return dontkeep
	tokeep	= notify_utmsg_cb( bt_utmsg_event_t::build_doregister() );
	DBG_ASSERT( tokeep == true );
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set a new value for the 'faststart'
 */ 
void	bt_utmsg_fstart_t::local_fstart(bool new_value)				throw()
{
	// log to debug
	KLOG_DBG("enter new_value=" << new_value);
	// if the new_value is equal to the current value, do nothing
	if( local_fstart() == new_value )	return;
	// update the current value, with the new one
	m_local_fstart	= new_value;
	
	// build the bt_cmd_t to report the new value to all remote peers  
	bt_cmd_t bt_cmd	= generate_xmit_cmd();
	// send this bt_cmd_t to all bt_utmsg_fstart_cnx_t
	std::list<bt_utmsg_fstart_cnx_t *>::iterator	iter;
	for(iter = cnx_db.begin(); iter != cnx_db.end(); iter++){
		bt_utmsg_fstart_cnx_t *	fstart_cnx	= *iter;
		// send this bt_cmd_t to this fstart_cnx
		fstart_cnx->send_cmd(bt_cmd);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Generate the bt_cmd_t to xmit
 */
bt_cmd_t	bt_utmsg_fstart_t::generate_xmit_cmd()				throw()
{
	// build the dvar
	dvar_t	dvar	= dvar_map_t();

	// insert the "b" field in the dvar
	dvar.map().insert("v", dvar_int_t(local_fstart()));

	// generate the payload of the bt_cmd_t
	pkt_t	payload;
	payload << bt_utmsgtype_t(bt_utmsgtype_t::FSTART);
	payload.append(datum_t(bencode_t::from_dvar(dvar)));
	// return the bt_cmd_t to xmit
	return bt_cmd_t::build_utmsg_payl(payload.to_datum(datum_t::NOCOPY));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    bt_utmsg_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the string which identify this bt_utmsg_vapi_t
 */
std::string	bt_utmsg_fstart_t::utmsgstr()		const throw()
{
#if 0
	return "fstart";
#else	// 'scramble' the string to make it harder to guess
	// - usefull for the 'hidding' of this options
	return bt_id_t("fstart").to_canonical_string();
#endif
}

/** \brief Construct a bt_utmsg_fstart_cnx_t from a bt_swarm_full_utmsg_t
 */
bt_utmsg_cnx_vapi_t * bt_utmsg_fstart_t::cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	bt_err_t	bt_err;	
	// create a bt_utmsg_fstart_cnx_t for this bt_swarm_full_utmsg_t
	bt_utmsg_fstart_cnx_t *fstart_cnx;
	fstart_cnx	= nipmem_new bt_utmsg_fstart_cnx_t();
	bt_err		= fstart_cnx->start(this, full_utmsg);
	DBG_ASSERT( bt_err.succeed() );
	// return the just build fstart_cnx
	return fstart_cnx;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the bt_utmsg_event_t
 */
bool bt_utmsg_fstart_t::notify_utmsg_cb(const bt_utmsg_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( utmsg_cb );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*utmsg_cb);
	// notify the caller
	bool tokeep = utmsg_cb->neoip_bt_utmsg_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END






/*! \file
    \brief Class to handle the bt_utmsg_punavail_t

\par Brief Description
\rev bt_utmsg_punavail_t is an extension which report the amount of byte taken and 
given on a particular bt_swarm_t.

\par NOTES
- it would be nice to save the bt_swarm_stats_t in the resumedata
  - thus it would make the dload/uload_len persistent accross reboot
- btw the dload/uload are currently counted per swarm
  - so giving to a particular swarm wont help taking on another

*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_punavail.hpp"
#include "neoip_bt_utmsg_punavail_cnx.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_utmsg_event.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_utmsg_punavail_t::bt_utmsg_punavail_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	utmsg_cb	= NULL;
}

/** \brief Destructor
 */
bt_utmsg_punavail_t::~bt_utmsg_punavail_t()		throw()
{
	// unregister this object
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( utmsg_cb ){
		bool	tokeep	= notify_utmsg_cb( bt_utmsg_event_t::build_unregister() );
		DBG_ASSERT( tokeep == true );
	}
	// close all pending bt_utmsg_punavail_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_utmsg_punavail_t::start(bt_swarm_t *bt_swarm, bt_utmsg_cb_t *utmsg_cb
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
//                    bt_utmsg_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the string which identify this bt_utmsg_vapi_t
 */
std::string	bt_utmsg_punavail_t::utmsgstr()		const throw()
{
#if 0
	return "punavail";
#else	// 'scramble' the string to make it harder to guess
	return bt_id_t("punavail").to_canonical_string();
#endif
}

/** \brief Construct a bt_utmsg_punavail_cnx_t from a bt_swarm_full_utmsg_t
 */
bt_utmsg_cnx_vapi_t * bt_utmsg_punavail_t::cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	bt_err_t	bt_err;	
	// create a bt_utmsg_punavail_cnx_t for this bt_swarm_full_utmsg_t
	bt_utmsg_punavail_cnx_t *punavail_cnx;
	punavail_cnx	= nipmem_new bt_utmsg_punavail_cnx_t();
	bt_err		= punavail_cnx->start(this, full_utmsg);
	DBG_ASSERT( bt_err.succeed() );
	// return the just build punavail_cnx
	return punavail_cnx;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the bt_utmsg_event_t
 */
bool bt_utmsg_punavail_t::notify_utmsg_cb(const bt_utmsg_event_t &event)	throw()
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






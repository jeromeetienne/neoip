/*! \file
    \brief Declaration of the kad_publish_t
  
*/

/* system include */
/* local include */
#include "neoip_kad_publish.hpp"
#include "neoip_kad_store.hpp"
#include "neoip_kad_delete.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_publish_t::kad_publish_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero the field
	kad_peer	= NULL;
	kad_store	= NULL;
}

/** \brief Desstructor
 */
kad_publish_t::~kad_publish_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete kad_store_t if needed
	nipmem_zdelete kad_store;
#if 0
/* TODO some race condition appears here
 * - as the caller doesnt know when this kad_delete_t is completed
 * - if the caller republish the same recid, both can 'crossover' and the kad_rec_t
 *   from kad_publish_t may be delete by the previous kad_delete_t
 * - BUG BUG what is bad is that in this case, it goes 100% cpu for an unknown reason!!!1
 *   - even ctrl-c no more work!!!!
 * - in fact the 100% issue is not due to the normal/expected cross over as the recid of 
 *   the record i use is always new random at every publish
 * - maybe it is due to the NULL callback ?!?! well seems unlikely
 * - maybe due to some other parts when it is supposed to find the record and fails to find it
 *   so it goes 100% 
 * - i dunno... find a way to make it crash ?
 */
	// launch a kad_delete_t for all published record - with no callback for autodelete on completion
	std::list<kad_recid_t>::iterator	iter;
	kad_delete_t *				kad_delete;
	kad_err_t				kad_err;
	for(iter = recid_db.begin(); iter != recid_db.end(); iter++){
		kad_recid_t	kad_recid	= *iter;
		// launch a kad_delete_t for this kad_recid_t/kad_keyid_t
		kad_delete	= nipmem_new kad_delete_t();
		// TODO to put this delay in the profile
		kad_err		= kad_delete->start(kad_peer, kad_recid, kad_keyid,delay_t::from_sec(2*60)
										, NULL, NULL);
		// if it failed, log the event and do nothing else
		// - in anycase it is in a destructor so nothing can be done
		if( kad_err.failed() )	KLOG_ERR("cant kad_delete_t due to " << kad_err);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                    Setup function
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
kad_err_t	kad_publish_t::start(kad_peer_t *kad_peer, const kad_recdups_t &kad_recdups
						, kad_publish_cb_t *callback, void *userptr) throw()
{
	kad_err_t	kad_err;
	// copy some parameters
	this->kad_peer		= kad_peer;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// store the record identification to delete them later
	kad_keyid	= kad_recdups.get_keyid();
	for(size_t i = 0; i < kad_recdups.size(); i++ )	recid_db.push_back(kad_recdups[i].get_recid());

	// start the kad_store_t
	kad_store	= nipmem_new kad_store_t();
	kad_err		= kad_store->start(kad_peer, kad_recdups, delay_t::NEVER, this, NULL);
	if( kad_err.failed() )	return kad_err;	
	// return no error
	return kad_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_store_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_store_t has an event to notify
 */
bool 	kad_publish_t::neoip_kad_store_cb(void *cb_userptr, kad_store_t &cb_kad_store
						, const kad_event_t &kad_event)		throw()
{
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the event MUST be store_ok()
	DBG_ASSERT( kad_event.is_store_ok() );

	// destroy the kad_store and mark it unused
	nipmem_zdelete kad_store;
			
	// simply forward the kad_event_t to the caller
	notify_callback( kad_event );
	// NOTE: here the tokeep is ignored
	return false;
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
bool 	kad_publish_t::notify_callback(const kad_event_t &kad_event)	throw()
{
	// if the callback is NULL, autodelete
	if( !callback ){
		nipmem_delete this;
		return false;
	}	
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_publish_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END



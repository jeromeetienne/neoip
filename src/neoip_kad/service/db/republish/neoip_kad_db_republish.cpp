/*! \file
    \brief Definition of the \ref kad_db_republish_t

*/

/* system include */
/* local include */
#include "neoip_kad_db_republish.hpp"
#include "neoip_kad_db.hpp"
#include "neoip_kad_store.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_db_republish_t::kad_db_republish_t(kad_db_t *kad_db)	throw()
{
	// copy the parameter
	this->kad_db	= kad_db;
}

/** \brief Constructor by default
 */
kad_db_republish_t::~kad_db_republish_t()	throw()
{
	// delete all the pending kad_store_t 
	while( !kad_store_db.empty() ){
		nipmem_delete	kad_store_db.front();
		kad_store_db.remove( kad_store_db.front() );	
	}	
}

	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    republish_rec function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief republish a record
 */
kad_err_t	kad_db_republish_t::republish_rec(const kad_rec_t &kad_rec)	throw()
{
	const kad_db_profile_t &profile	= kad_db->get_profile();
	kad_peer_t *	kad_peer	= kad_db->kad_peer;
	kad_store_t *	kad_store;
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter");

	// start a kad_store_t for this kad_rec_t
	// - with local_storage = false not to modify the local database
	kad_store	= nipmem_new kad_store_t();
	kad_err		= kad_store->start(kad_peer, kad_rec, profile.republish_store_timeout()
								, this, NULL, false);
	// handle the error
	if( kad_err.failed() ){
		nipmem_delete kad_store;
		return kad_err;
	}
	// link it to the database
	kad_store_db.push_back(kad_store);

	// return no error
	return kad_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      kad_store_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_store_t has an event to notify
 */
bool kad_db_republish_t::neoip_kad_store_cb(void *cb_userptr, kad_store_t &cb_kad_store
						, const kad_event_t &kad_event)	throw()
{
	kad_store_t *	kad_store	= &cb_kad_store;
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the event MUST be store_ok()
	DBG_ASSERT( kad_event.is_store_ok() );

	// remove this publish from the database and delete it
	kad_store_db.remove( kad_store );
	nipmem_delete	kad_store;

	// if this republish failed, log and exit
	if( kad_event.is_fatal() ){
		KLOG_INFO("REPUBLISHING failed due to " << kad_event);
		return false;
	}
	
	// NOTE: here the kad_store_t succeed. no action to take

	// NOTE: dont keep the kad_store_t as it is deleted
	return false;
}

NEOIP_NAMESPACE_END;







/*! \file
    \brief Definition of the kad_rec_src_t

*/

/* system include */
#include <stdlib.h>
/* local include */
#include "neoip_kad_rec_src.hpp"
#include "neoip_kad_query.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_rec_src_t::kad_rec_src_t()		throw()
{
	// zero some field
	kad_peer	= NULL;
	kad_query	= NULL;
}

/** \brief Desstructor
 */
kad_rec_src_t::~kad_rec_src_t()		throw()
{
	// delete kad_query_t if needed
	nipmem_zdelete	kad_query;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation for this object
 */
kad_err_t	kad_rec_src_t::start(kad_peer_t *kad_peer, const kad_keyid_t &keyid
					, kad_rec_src_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_ERR("enter callback " << neoip_cpp_typename(*callback) );	
	// copy the parameter
	this->kad_peer		= kad_peer;
	this->keyid		= keyid;
	this->callback		= callback;
	this->userptr		= userptr;
	// set some parameter
	notify_inprogress	= false;
	want_more		= false;
	// return no error
	return kad_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Misc function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Warn the kad_rec_src_t that more addresses are wished
 * 
 * - it may be called several times in a row, is the same as calling it only once
 *   - it just indicate the wish of the caller to get_more()
 * - NOTE: this is ok to use it during a neoip_kad_rec_src_cb_t callback 
 */
void	kad_rec_src_t::get_more()		throw()
{
	// if the query is not yet launched, do it here
	if( kad_query == NULL ){
		kad_err_t	kad_err;
		kad_query	= nipmem_new kad_query_t();
		kad_err		= kad_query->start(kad_peer, keyid, 0, delay_t::NEVER, this, NULL);
		DBG_ASSERT( kad_err.succeed() );
	}

	// set the want_more
	want_more	= true;

	// launch the delivery process if not currently running
	if( !notify_inprogress )	zerotimer.append(this, NULL);
}

/** \brief Push back this address into the address source such as it will be 
 *         the next address notified in the callback.
 * 
 * - NOTE: this is ok to use it during a neoip_kad_rec_src_cb_t callback 
 */
void	kad_rec_src_t::push_back(const kad_rec_t &record)	throw()
{
	// push the record at the begining of record_db
	record_db.push_front(record);
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       kad_query_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when kad_query_t has an event to notify
 */
bool	kad_rec_src_t::neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << kad_event);
	// sanity check - the kad_event MUST be query_ok()
	DBG_ASSERT( kad_event.is_query_ok() );

	// handle the kad_event depending on its value
	switch( kad_event.get_value() ){
	case kad_event_t::RECDUPS:{
			const kad_recdups_t &	kad_recdups	= kad_event.get_recdups(NULL);
			// push all the kad_rec_t into the record_db
			for( size_t i = 0; i < kad_recdups.size(); i++ ){
				record_db.push_back(kad_recdups[i]);
			}
			break;}
	case kad_event_t::TIMEDOUT:
			break;
	default:	DBG_ASSERT( 0 );
	}

	// push a null kad_rect in the record_db to mark the end of the query
	record_db.push_back(kad_rec_t());

	// delete the kad_query_t and mark it unused
	nipmem_zdelete kad_query;

	// if the caller is currently want_more, launch a delivery
	// NOTE: this function may delete the object, dont use it beyong this point	
	if( want_more )	do_delivery();

	// return 'dontkeep'
	return false;	
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	kad_rec_src_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw()
{
	// sanity check - want_more MUST be true
	DBG_ASSERT( want_more );

	// call the main deliver function
	// NOTE: this function may delete the object, dont use it beyong this point
	do_delivery();

	// return 'tokeep'
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        Internal function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Main function to deliver the items
 * 
 * @return a 'tokeep'
 */
bool	kad_rec_src_t::do_delivery()	throw()
{
	// sanity check - want_more MUST be true
	DBG_ASSERT( want_more );
	
	// set the notify_inprogress
	notify_inprogress = true;
	
	// loop until either the caller dont want_more or if there is no more record available
	while( want_more && !record_db.empty() ){
		// clear the want_more
		want_more	= false;
		// get the front record to deliver from the record_db
		kad_rec_t	record	= record_db.front();
		// remove the front record from the record_db
		record_db.pop_front();
		// backup the tokey_check_t context to check after the callback notification
		TOKEEP_CHECK_BACKUP_DFL(*callback);
		// notify the callback of the current address
		bool tokeep	= callback->neoip_kad_rec_src_cb(userptr, *this, record);
		// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
		TOKEEP_CHECK_MATCH_DFL(tokeep);	
		// if the object has been deleted, return now
		if( !tokeep )	return false;
	}

	// clear the notify_inprogress
	notify_inprogress = false;

	// return 'dontkeep'
	return false;
}

NEOIP_NAMESPACE_END



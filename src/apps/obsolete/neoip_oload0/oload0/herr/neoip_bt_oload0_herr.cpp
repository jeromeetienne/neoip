/*! \file
    \brief Definition of the \ref bt_oload0_herr_t

\par Brief Description
\ref bt_oload0_herr_t simply hold the bt_httpo_full_t while returning error 
to it. when a bt_httpo_full_t notify a bt_httpo_event_t::CNX_CLOSED, it is deleted.
- this is mainly a workaround a bug in the socket tcp layer which doesnt handle
  a linger properly
  - only the socket tcp layer is able to handle linger, simply forward the http_rephd_t
    describing the error and close the bt_httpo_full_t. The linger will take care
    of holding the connection until it http_rephd_t is actually sent to the remote
    peer.

*/

/* system include */
/* local include */
#include "neoip_bt_oload0_herr.hpp"
#include "neoip_bt_oload0.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
bt_oload0_herr_t::bt_oload0_herr_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
}

/** \brief Destructor
 */
bt_oload0_herr_t::~bt_oload0_herr_t()	throw()
{
	// delete the httpo_full_db if needed
	while( !httpo_full_db.empty() ){
		nipmem_delete	httpo_full_db.front();
		httpo_full_db.pop_front();
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t bt_oload0_herr_t::start(bt_oload0_t *bt_oload0)	throw()
{
	// copy the parameter
	this->bt_oload0	= bt_oload0;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a new bt_httpo_full_t to this object
 * 
 * - NOTE: when this function is called, it takes the ownership of the bt_httpo_full_t 
 *   so the bt_httpo_full_t may be deleted during this function
 */
void	bt_oload0_herr_t::add_httpo_full(bt_httpo_full_t *httpo_full, const http_rephd_t &http_rephd)	throw()
{
	// log to debug
	KLOG_WARN("enter http_rephd=" << http_rephd);

	// start this bt_httpo_full_t with no bt_swarm_t and an null file_range_t
	bt_err_t bt_err	= httpo_full->start(NULL, http_rephd, file_range_t(), this, NULL);
	if( bt_err.failed() ){
		// log the event
		KLOG_INFO("httpo_full failed to start due to " << bt_err);
		// delete this httpo_full
		nipmem_zdelete httpo_full;
		return;
	}
	// link this bt_httpo_full_t to the httpo_full_db
	httpo_full_db.push_back(httpo_full);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bt_httpo_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_httpo_full_t when to notify an event
 * 
 * @return true if the bt_httpo_full_t is still valid after the callback
 */
bool	bt_oload0_herr_t::neoip_bt_httpo_full_cb(void *cb_userptr, bt_httpo_full_t &cb_bt_httpo_full
					, const bt_httpo_event_t &httpo_event)	throw()
{
	bt_httpo_full_t*httpo_full	= &cb_bt_httpo_full;
	// log to debug
	KLOG_WARN("enter httpo_event=" << httpo_event);
	// sanity check - bt_httpo_event_t MUST be is_full_ok()
	DBG_ASSERT( httpo_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( httpo_event.get_value() ){
	case bt_httpo_event_t::CNX_CLOSED:
			// remove it from the httpo_full_db
			httpo_full_db.remove(httpo_full);
			// delete the bt_httpo_full_t object itself
			nipmem_zdelete httpo_full;
			// return dontkeep - as the httpo_full has just been deleted
			return false;
	default:	DBG_ASSERT(0);
	}		
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END;





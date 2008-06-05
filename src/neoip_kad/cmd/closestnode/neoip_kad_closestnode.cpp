/*! \file
    \brief Declaration of the kad_closestnode_t

- from the paper, "To store a key,value pair, a participant locates 
  the k closest nodes to the key and sends them store RPCs."
  
*/

/* system include */
/* local include */
#include "neoip_kad_closestnode.hpp"
#include "neoip_kad_nsearch.hpp"
#include "neoip_kad_peer.hpp"
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
kad_closestnode_t::kad_closestnode_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero the field
	kad_nsearch	= NULL;
}

/** \brief Desstructor
 */
kad_closestnode_t::~kad_closestnode_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete kad_nsearch_t if needed
	nipmem_zdelete kad_nsearch;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                    Setup function
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/** \brief Set the kad_addr_arr_t to start with
 * 
 * - not mandatory
 * - if done, MUST be done before start()
 */
kad_closestnode_t &kad_closestnode_t::set_base_addr_arr(const kad_addr_arr_t &base_addr_arr)	throw()
{
	// copy the parameter
	this->base_addr_arr	= base_addr_arr;
	// return the object itself
	return *this;
}

/** \brief Start the action
 */
kad_err_t	kad_closestnode_t::start(kad_peer_t *kad_peer, const kad_targetid_t &kad_targetid
					, size_t max_nb_node, const delay_t &expire_delay
					, kad_closestnode_cb_t *callback, void *userptr) 	throw()
{
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter callback=" << callback << "("<< neoip_cpp_typename(*callback) << ")" );	
	// copy some parameters
	this->callback	= callback;
	this->userptr	= userptr;
	
	// start the kad_nsearch_t
	// - from the paper, "To store a key,value pair, a participant locates 
	//   the k closest nodes to the key and sends them store RPCs."	
	kad_nsearch	= nipmem_new kad_nsearch_t(this, NULL, kad_peer);
	kad_err		= kad_nsearch->start_findnode(kad_targetid, max_nb_node, expire_delay
								, base_addr_arr);
	if( kad_err.failed() )	return kad_err;	

	// return no error
	return kad_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        kad_nsearch_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_nsearch_t has an event to notify
 */
bool 	kad_closestnode_t::neoip_kad_nsearch_cb(void *cb_userptr, kad_nsearch_t &cb_kad_nsearch
					, const kad_event_t &kad_event)		throw()
{
	// log to debug
	KLOG_DBG("kad_event=" << kad_event);
	// sanity check - the event MUST be nsearch_findnode_ok()
	DBG_ASSERT( kad_event.is_nsearch_findnode_ok() );
	
	// just forward the kad_event_t
	return notify_callback(kad_event);
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
bool 	kad_closestnode_t::notify_callback(const kad_event_t &kad_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_closestnode_cb(userptr, *this, kad_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END



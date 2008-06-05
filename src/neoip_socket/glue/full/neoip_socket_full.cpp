/*! \file
    \brief Definition of the socket_full_t
    
*/


/* system include */
/* local include */
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"

// include to access domain specific api
#include "neoip_socket_full_udp.hpp"
#include "neoip_socket_full_ntlay.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref socket_full_t constant
const size_t	socket_full_t::UNLIMITED	= socket_full_t::UNLIMITED_VAL;
// end of constants definition

// definition of the factory plant for socket_full_vapi_t 
FACTORY_PLANT_DEFINITION (socket_full_factory, socket_domain_t, socket_full_vapi_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_full_t::socket_full_t(socket_full_vapi_t *p_full_vapi) throw()
{
	// copy the parameter
	m_full_vapi	= p_full_vapi;
}

/** \brief Destructor
 */
socket_full_t::~socket_full_t() throw()
{
	// warn the socket_full_vapi_t of the destruction of the glue
	// - NOTE: the socket_full_vapi_t is not directly deleted here, because the domain specific
	//   handling of the socket may still require some time.
	//   - e.g. if the domain specific socket full is handling its own close protocol or is
	//     reliable with a linger, the socket will still exist until those protocols are
	//     completed.
	//   - so they can be deleted here
	full_vapi()->notify_glue_destruction();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            Setup Function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the callback parameter
 */
socket_full_t &	socket_full_t::set_callback(socket_full_cb_t *callback, void *userptr)	throw()
{
	// sanity check - the full_vapi MUST be initialized
	DBG_ASSERT(m_full_vapi);
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// set the socket_full_vapi_t callback to report to this socket_full_t
	full_vapi()->set_callback(this, NULL);
	// return the object itself
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_vapi_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_vapi_t to provide event
 * 
 * - note: just forward the socket_event_t from the socket_full_vapi_t
 */
bool	socket_full_t::neoip_socket_full_vapi_cb(void *userptr, socket_full_vapi_t &cb_full_vapi
						, const socket_event_t &socket_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// backup the object_slotid of the socket_full_t - to be able to return its tokeep value
	slot_id_t	full_vapi_slotid	= m_full_vapi->get_object_slotid();
	
	// notify the socket_event_t to the caller
	notify_callback(socket_event);
	
	// if the socket_full_vapi_t has no been deleted, so 'tokeep' else return 'dontkeep'
	// - it may happen when the socket_full_t is deleted by the caller but the _vapi_t level
	//   still needs to handle stuff (e.g. linger or closure protocol)
	return object_slotid_tokeep(full_vapi_slotid);
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
bool 	socket_full_t::notify_callback(const socket_event_t &socket_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_socket_full_event_cb(userptr, *this, socket_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


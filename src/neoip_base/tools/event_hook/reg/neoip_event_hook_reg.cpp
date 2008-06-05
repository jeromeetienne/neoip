/*! \file
    \brief Definition of the \ref event_hook_reg_t

\par Brief Description
event_hook_reg_t is a mechanism for code to be called when the lib_session_t
is about to exit. e.g. to unregister a resource or delete file.

\par About the local states
- the event_hook_reg_t is in_exiting() as soon as begin_exiting() is called
  and the caller will be notified by a zerotimer_t launched in the begin_exiting()
- the event_hook_reg_t is is_notified() as soon as the caller has been notified

\par about the exit_order
- there is a 'exit_order' which allow to determine the order at which the 
  various event_hook_reg_t will be called.
- the event_hook_reg_t are notified in increasing exit_order
- multiple event_hook_reg_t may have the same exit_order
- example:
  - layer A and B have an exit_order of 4
  - layer C have an exit_order of 2
  - layer C will be notified first, and ONLY when layer C has deleted its 
    event_hook_reg_t, layer A and B will be notified.
    - layer A and B will be notified in the same event loop because they have
      the same exit_order 

\par About expiration of a specific event_hook_reg_t
- the lib_session_t exiting has already a global timeout.
- sometime it is insteresting to timeout a given event_hook_reg_t to let 
  time to the others event_hook_reg_t with a greater exit_order.
- if it is wished it must be coded inside the event_hook_reg_t caller 
  - this is not coded here on purpose because it is IMPORTANT that the caller
    is fully aware of the end of the event_hook_reg_t 
  - it may be mandatory to delete some rescources at one exit_order BEFORE
    going to the next. so the caller is the one deleting its event_hook_reg
    to explicitly show that it is ok to continue the lib_session_t exiting process.

*/

/* system include */
/* local include */
#include "neoip_event_hook_reg.hpp"
#include "neoip_event_hook.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
event_hook_reg_t::event_hook_reg_t()	throw()
{
	// zero some field
	m_event_hook	= NULL;
}

/** \brief Destructor
 */
event_hook_reg_t::~event_hook_reg_t()	throw()
{
	// unlink this object from the lib_session_t
	if( m_event_hook )	m_event_hook->remove(hook_level(), this, NULL);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
void	event_hook_reg_t::start(int p_hook_level, event_hook_t *p_event_hook
			, event_hook_reg_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->m_event_hook	= p_event_hook;
	this->m_hook_level	= p_hook_level;
	this->callback		= callback;
	this->userptr		= userptr;
	// append this object to the event_hook_t
	m_event_hook->append(hook_level(), this, NULL);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			event_hook_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a event_hook_t level is notified
 * 
 * @return a 'tokeep' for the event_hook_t object
 */
bool	event_hook_reg_t::neoip_event_hook_notify_cb(void *userptr
			, const event_hook_t *cb_event_hook, int p_hook_level)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// sanity check - notified hook_level MUST be equal to the local one
	DBG_ASSERT( p_hook_level == hook_level() );
	
	// forward the notification to the caller
	// TODO some issue here with the tokeep to return
	// - how do i know if event_hook has been deleted or not ?
	// - do i say "unable to delete during callback" ?
	//   - currently this is the case. 
	//   - a object_id_t somewhere  may make it possible. i dunno where
	//     for now and my brain is off 
	notify_callback();

	// return tokeep all the time	
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback for the caller
 */
bool event_hook_reg_t::notify_callback()	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_event_hook_reg_cb(userptr, *this);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;





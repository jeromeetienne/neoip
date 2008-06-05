/*! \file
    \brief Header of the \ref event_hook_t callback

*/


#ifndef __NEOIP_EVENT_HOOK_CB_HPP__ 
#define __NEOIP_EVENT_HOOK_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class event_hook_t;

/** \brief the callback class for event_hook_t
 */
class event_hook_cb_t {
public:
	/** \brief called when a event_hook_t level is notified
	 * 
	 * - NOTE: the event hook can be modified during the callback
	 * 
	 * @return a 'tokeep' for the event_hook_t object
	 */
	virtual bool neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
							, int hook_level)	throw() = 0;
	virtual ~event_hook_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_EVENT_HOOK_CB_HPP__  */




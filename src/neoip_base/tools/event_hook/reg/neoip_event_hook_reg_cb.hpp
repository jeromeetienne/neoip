/*! \file
    \brief Header of the \ref event_hook_reg_t callback

*/


#ifndef __NEOIP_EVENT_HOOK_REG_CB_HPP__ 
#define __NEOIP_EVENT_HOOK_REG_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class event_hook_reg_t;

/** \brief the callback class for event_hook_reg_t
 */
class event_hook_reg_cb_t {
public:
	/** \brief called when a event_hook_reg_t level is notified
	 * 
	 * @return a 'tokeep' for the event_hook_reg_t object
	 */
	virtual bool neoip_event_hook_reg_cb(void *userptr
				, event_hook_reg_t &cb_hook_reg)	throw() = 0;
	virtual ~event_hook_reg_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_EVENT_HOOK_REG_CB_HPP__  */




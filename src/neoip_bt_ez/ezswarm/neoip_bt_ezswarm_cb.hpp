/*! \file
    \brief Declaration of the bt_ezswarm_t
    
*/


#ifndef __NEOIP_BT_EZSWARM_CB_HPP__ 
#define __NEOIP_BT_EZSWARM_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_ezswarm_t;
class	bt_ezswarm_event_t;

/** \brief the callback class for bt_ezswarm_t
 */
class bt_ezswarm_cb_t {
public:
	/** \brief callback notified by \ref bt_ezswarm_t when to notify an event
	 * 
	 * @return true if the bt_ezswarm_t is still valid after the callback
	 */
	virtual bool neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw() = 0;
	// virtual destructor
	virtual ~bt_ezswarm_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_CB_HPP__  */




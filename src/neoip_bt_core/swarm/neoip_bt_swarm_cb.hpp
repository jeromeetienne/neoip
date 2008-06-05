/*! \file
    \brief Declaration of the bt_swarm_t
    
*/


#ifndef __NEOIP_BT_SWARM_CB_HPP__ 
#define __NEOIP_BT_SWARM_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_swarm_t;
class	bt_swarm_event_t;

/** \brief the callback class for bt_swarm_t
 */
class bt_swarm_cb_t {
public:
	/** \brief callback notified by \ref bt_swarm_t when to notify an event
	 * 
	 * @return true if the bt_swarm_t is still valid after the callback
	 */
	virtual bool neoip_bt_swarm_cb(void *cb_userptr, bt_swarm_t &cb_bt_swarm
					, const bt_swarm_event_t &swarm_event)	throw() = 0;
	// virtual destructor
	virtual ~bt_swarm_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_CB_HPP__  */




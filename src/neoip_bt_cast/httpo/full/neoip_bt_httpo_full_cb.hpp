/*! \file
    \brief Declaration of the bt_httpo_full_t
    
*/


#ifndef __NEOIP_BT_HTTPO_FULL_CB_HPP__ 
#define __NEOIP_BT_HTTPO_FULL_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_httpo_full_t;
class	bt_httpo_event_t;

/** \brief the callback class for bt_httpo_full_t
 */
class bt_httpo_full_cb_t {
public:
	/** \brief callback notified by \ref bt_httpo_full_t when to notify an event
	 * 
	 * @return true if the bt_httpo_full_t is still valid after the callback
	 */
	virtual bool neoip_bt_httpo_full_cb(void *cb_userptr, bt_httpo_full_t &cb_bt_httpo_full
					, const bt_httpo_event_t &httpo_event)	throw() = 0;
	// virtual destructor
	virtual ~bt_httpo_full_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPO_FULL_CB_HPP__  */




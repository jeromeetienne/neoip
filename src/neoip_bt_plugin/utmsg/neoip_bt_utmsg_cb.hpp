/*! \file
    \brief Declaration of the bt_utmsg_t
    
*/


#ifndef __NEOIP_BT_UTMSG_CB_HPP__ 
#define __NEOIP_BT_UTMSG_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_utmsg_vapi_t;
class	bt_utmsg_event_t;

/** \brief the callback class for bt_utmsg_t
 */
class bt_utmsg_cb_t {
public:
	/** \brief callback notified by \ref bt_utmsg_vapi_t when to notify an event
	 * 
	 * @return true if the bt_utmsg_t is still valid after the callback
	 */
	virtual bool neoip_bt_utmsg_cb(void *cb_userptr, bt_utmsg_vapi_t &cb_utmsg_vapi
					, const bt_utmsg_event_t &utmsg_event)	throw() = 0;
	// virtual destructor
	virtual ~bt_utmsg_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_CB_HPP__  */




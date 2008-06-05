/*! \file
    \brief Declaration of the bt_peersrc_t
    
*/


#ifndef __NEOIP_BT_PEERSRC_CB_HPP__ 
#define __NEOIP_BT_PEERSRC_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_peersrc_vapi_t;
class	bt_peersrc_event_t;

/** \brief the callback class for bt_peersrc_t
 */
class bt_peersrc_cb_t {
public:
	/** \brief callback notified by \ref bt_peersrc_vapi_t when to notify an event
	 * 
	 * @return true if the bt_peersrc_t is still valid after the callback
	 */
	virtual bool neoip_bt_peersrc_cb(void *cb_userptr, bt_peersrc_vapi_t &cb_peersrc_vapi
					, const bt_peersrc_event_t &peersrc_event)	throw() = 0;
	// virtual destructor
	virtual ~bt_peersrc_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_CB_HPP__  */




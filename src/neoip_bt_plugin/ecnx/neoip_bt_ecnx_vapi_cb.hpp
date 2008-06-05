/*! \file
    \brief Declaration of the bt_ecnx_t
    
*/


#ifndef __NEOIP_BT_ECNX_VAPI_CB_HPP__ 
#define __NEOIP_BT_ECNX_VAPI_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_ecnx_vapi_t;
class	bt_ecnx_event_t;

/** \brief the callback class for bt_ecnx_vapi_t
 */
class bt_ecnx_vapi_cb_t {
public:
	/** \brief callback notified by \ref bt_ecnx_vapi_t when to notify an event
	 * 
	 * @return true if the bt_ecnx_t is still valid after the callback
	 */
	virtual bool neoip_bt_ecnx_vapi_cb(void *cb_userptr, bt_ecnx_vapi_t &cb_ecnx_vapi
					, const bt_ecnx_event_t &ecnx_event)	throw() = 0;
	// virtual destructor
	virtual ~bt_ecnx_vapi_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_ECNX_VAPI_CB_HPP__  */




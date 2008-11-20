/*! \file
    \brief Declaration of the bt_scasti_t

*/


#ifndef __NEOIP_BT_SCASTI_CB_HPP__
#define __NEOIP_BT_SCASTI_CB_HPP__

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_scasti_vapi_t;
class	bt_scasti_event_t;

/** \brief the callback class for bt_scasti_t
 */
class bt_scasti_cb_t {
public:
	/** \brief callback notified by \ref bt_scasti_t
	 */
	virtual bool neoip_bt_scasti_cb(void *cb_userptr, bt_scasti_vapi_t &cb_scasti_vapi
				, const bt_scasti_event_t &scasti_event)	throw() = 0;
	// virtual destructor
	virtual ~bt_scasti_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SCASTI_CB_HPP__  */




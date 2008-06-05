/*! \file
    \brief Declaration of the bt_tracker_client_t
    
*/


#ifndef __NEOIP_BT_TRACKER_CLIENT_CB_HPP__ 
#define __NEOIP_BT_TRACKER_CLIENT_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_tracker_client_t;
class	bt_tracker_reply_t;
class	bt_err_t;

/** \brief the callback class for bt_tracker_client_t
 */
class bt_tracker_client_cb_t {
public:
	/** \brief callback notified by \ref bt_tracker_client_t when to notify an event
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param bt_tracker_client 	the bt_tracker_client_t which notified this callback
	 * @param bt_err	the bt_err matching the connection establishement
	 * @param reply		the bt_tracker_reply_t constaining the result (valid IIF bt_err.succeed())
	 * @return true if the bt_tracker_client_t is still valid after the callback
	 */
	virtual bool neoip_bt_tracker_client_cb(void *cb_userptr, bt_tracker_client_t &cb_bt_tracker_client
							, const bt_err_t &bt_err
							, const bt_tracker_reply_t &reply)	throw() = 0;
	// virtual destructor
	virtual ~bt_tracker_client_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_TRACKER_CLIENT_CB_HPP__  */




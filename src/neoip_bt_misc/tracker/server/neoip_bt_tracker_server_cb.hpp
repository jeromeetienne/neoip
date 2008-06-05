/*! \file
    \brief Declaration of the bt_tracker_server_t
    
*/


#ifndef __NEOIP_BT_TRACKER_SERVER_CB_HPP__ 
#define __NEOIP_BT_TRACKER_SERVER_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_slotpool_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_tracker_server_t;
class	bt_tracker_request_t;
class	bt_tracker_reply_t;

/** \brief the callback class for bt_tracker_server_t
 */
class bt_tracker_server_cb_t {
public:
	/** \brief callback notified by \ref bt_tracker_server_t when to notify an request
	 * 
	 * @param userptr  		the userptr associated with this callback
	 * @param bt_tracker_server 	the bt_tracker_server_t which notified this callback
	 * @param request		the bt_tracker_request_t constaining the request to handle
	 * @param reply			the bt_tracker_reply_t which is the insync reply, if not null after
	 *				the callback
	 * @return true if the bt_tracker_server_t is still valid after the callback
	 */
	virtual bool neoip_bt_tracker_server_cb(void *cb_userptr, bt_tracker_server_t &cb_bt_tracker_server
						, slot_id_t cnx_slotid, const bt_tracker_request_t &request
						, bt_tracker_reply_t &reply_out)	throw() = 0;
	// virtual destructor
	virtual ~bt_tracker_server_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_TRACKER_SERVER_CB_HPP__  */




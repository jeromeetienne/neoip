/*! \file
    \brief Declaration of the bt_utmsg_bcast_handler_cb_t
    
*/


#ifndef __NEOIP_BT_UTMSG_BCAST_HANDLER_CB_HPP__ 
#define __NEOIP_BT_UTMSG_BCAST_HANDLER_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_utmsg_bcast_handler_t;
class	pkt_t;

/** \brief the callback class for bt_utmsg_t
 */
class bt_utmsg_bcast_handler_cb_t {
public:
	/** \brief callback notified by \ref bt_utmsg_bcast_t when to notify a recved pkt_t
	 * 
	 * @return true if the bt_utmsg_cast_handler_t is still valid after the callback
	 */
	virtual bool neoip_bt_utmsg_bcast_handler_cb(void *cb_userptr, bt_utmsg_bcast_handler_t &cb_bcast_handler
							, pkt_t &recved_pkt)	throw() = 0;
	// virtual destructor
	virtual ~bt_utmsg_bcast_handler_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_BCAST_HANDLER_CB_HPP__  */




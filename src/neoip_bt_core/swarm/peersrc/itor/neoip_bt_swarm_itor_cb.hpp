/*! \file
    \brief Declaration of the bt_swarm_itor_t
    
*/


#ifndef __NEOIP_BT_SWARM_ITOR_CB_HPP__ 
#define __NEOIP_BT_SWARM_ITOR_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_swarm_itor_t;
class	bt_err_t;
class	socket_full_t;
class	bytearray_t;
class	bt_handshake_t;

/** \brief the callback class for bt_swarm_itor_t
 */
class bt_swarm_itor_cb_t {
public:
	/** \brief callback notified by \ref bt_swarm_itor_t when to notify an event
	 * 
	 * @return true if the bt_swarm_itor_t is still valid after the callback
	 */
	virtual bool neoip_bt_swarm_itor_cb(void *cb_userptr, bt_swarm_itor_t &cb_swarm_itor
					, const bt_err_t &bt_err, socket_full_t *socket_full
					, const bytearray_t &recved_data
					, const bt_handshake_t &remote_handshake)	throw() = 0;
	// virtual destructor
	virtual ~bt_swarm_itor_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_ITOR_CB_HPP__  */




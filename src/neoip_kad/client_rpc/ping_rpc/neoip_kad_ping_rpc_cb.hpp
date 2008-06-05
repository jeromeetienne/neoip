/*! \file
    \brief Declaration of the kad_ping_rpc_cb_t
    
*/


#ifndef __NEOIP_KAD_PING_RPC_CB_HPP__ 
#define __NEOIP_KAD_PING_RPC_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class kad_ping_rpc_t;
class kad_event_t;

/** \brief the callback class for kad_ping_rpc_t
 */
class kad_ping_rpc_cb_t {
public:
	/** \brief callback notified when a kad_ping_rpc_t has an event to notify
	 */
	virtual bool neoip_kad_ping_rpc_cb(void *cb_userptr, kad_ping_rpc_t &cb_kad_ping_rpc
					, const kad_event_t &kad_event)	throw() = 0;
	virtual ~kad_ping_rpc_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_PING_RPC_HPP__ 




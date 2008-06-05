/*! \file
    \brief Declaration of the kad_findnode_rpc_cb_t
    
*/


#ifndef __NEOIP_KAD_FINDNODE_RPC_CB_HPP__ 
#define __NEOIP_KAD_FINDNODE_RPC_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class kad_findnode_rpc_t;
class kad_event_t;

/** \brief the callback class for kad_findnode_rpc_t
 */
class kad_findnode_rpc_cb_t {
public:
	/** \brief callback notified when a kad_findnode_rpc_t has an event to notify
	 */
	virtual bool neoip_kad_findnode_rpc_cb(void *cb_userptr, kad_findnode_rpc_t &cb_kad_findnode_rpc
					, const kad_event_t &kad_event)	throw() = 0;
	virtual ~kad_findnode_rpc_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_FINDNODE_RPC_HPP__ 




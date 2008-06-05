/*! \file
    \brief Declaration of the kad_client_rpc_cb_t
    
*/


#ifndef __NEOIP_KAD_CLIENT_RPC_VAPI_HPP__ 
#define __NEOIP_KAD_CLIENT_RPC_VAPI_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class kad_client_rpc_t;
class kad_event_t;
class pkt_t;

/** \brief the callback class for kad_client_rpc_t
 */
class kad_client_rpc_vapi_t {
public:
	/** \brief Called by kad_client_rpc_t to notify a reply payload
	 * 
	 * - this is forbidden to delete the notifier in this function
	 * 
	 * @return a kad_event_t. if it is non null, the event is notified, otherwise it is ignored
	 */
	virtual kad_event_t reply_payload_cb(pkt_t &reply_payload)	throw() = 0;

	/** \brief Called by kad_client_rpc_t to notify a kad_event_t
	 */
	virtual bool	notify_event(const kad_event_t &kad_event)	throw() = 0;

	//! the virtual destructor
	virtual ~kad_client_rpc_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_CLIENT_RPC_HPP__ 




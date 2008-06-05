/*! \file
    \brief Declaration of the kad_closestnode_cb_t
    
*/


#ifndef __NEOIP_KAD_CLOSESTNODE_CB_HPP__ 
#define __NEOIP_KAD_CLOSESTNODE_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class kad_closestnode_t;
class kad_event_t;

/** \brief the callback class for kad_closestnode_t
 */
class kad_closestnode_cb_t {
public:
	/** \brief callback notified when a kad_closestnode_t has an event to notify
	 */
	virtual bool neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
					, const kad_event_t &kad_event)	throw() = 0;
	virtual ~kad_closestnode_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_CLOSESTNODE_HPP__ 




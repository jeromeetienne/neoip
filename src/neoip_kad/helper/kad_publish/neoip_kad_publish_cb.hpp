/*! \file
    \brief Declaration of the kad_publish_cb_t
    
*/


#ifndef __NEOIP_KAD_PUBLISH_CB_HPP__ 
#define __NEOIP_KAD_PUBLISH_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class kad_publish_t;
class kad_event_t;

/** \brief the callback class for kad_publish_t
 */
class kad_publish_cb_t {
public:
	/** \brief callback notified when a kad_publish_t has an event to notify
	 */
	virtual bool neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
					, const kad_event_t &kad_event)	throw() = 0;
	virtual ~kad_publish_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_PUBLISH_HPP__ 




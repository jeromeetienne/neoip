/*! \file
    \brief Header of the \ref kad_nsearch_t
    
*/


#ifndef __NEOIP_KAD_NSEARCH_CB_HPP__ 
#define __NEOIP_KAD_NSEARCH_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_event.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class kad_nsearch_t;


/** \brief the callback class for kad_nsearch_t
 */
class kad_nsearch_cb_t {
public:
	/** \brief callback notified when a kad_nsearch_t has an event to notify
	 */
	virtual bool neoip_kad_nsearch_cb(void *cb_userptr, kad_nsearch_t &cb_kad_nsearch
						, const kad_event_t &kad_event)	throw() = 0;
	virtual ~kad_nsearch_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_NSEARCH_CB_HPP__  */











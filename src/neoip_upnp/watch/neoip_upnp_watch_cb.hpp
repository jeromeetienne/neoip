/*! \file
    \brief Declaration of the upnp_watch_t callback
    
*/


#ifndef __NEOIP_UPNP_WATCH_CB_HPP__ 
#define __NEOIP_UPNP_WATCH_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class upnp_watch_t;
class upnp_watch_event_t;

/** \brief the callback class for upnp_watch_t
 */
class upnp_watch_cb_t {
public:
	/** \brief callback notified by \ref upnp_watch_t to notify an event
	 */
	virtual bool neoip_upnp_watch_cb(void *cb_userptr, upnp_watch_t &cb_upnp_watch
					, const upnp_watch_event_t &watch_event)	throw() = 0;
	virtual ~upnp_watch_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_WATCH_CB_HPP__  */




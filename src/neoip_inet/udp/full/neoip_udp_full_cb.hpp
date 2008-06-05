/*! \file
    \brief Declaration of the udp_full_t
    
*/


#ifndef __NEOIP_UDP_FULL_CB_HPP__ 
#define __NEOIP_UDP_FULL_CB_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class udp_full_t;
class udp_event_t;

/** \brief the callback class for udp_full_t
 */
class udp_full_cb_t {
public:
	/** \brief callback notified by \ref udp_full_t when to notify an event
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param udp_full 	the udp_full_t which notified this callback
	 * @param udp_event	notified event
	 * @return true if the udp_full_t is still valid after the callback
	 */
	virtual bool neoip_inet_udp_full_event_cb(void *cb_userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw() = 0;
	virtual ~udp_full_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_FULL_CB_HPP__  */




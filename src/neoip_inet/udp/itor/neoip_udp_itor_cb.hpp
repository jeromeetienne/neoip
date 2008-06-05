/*! \file
    \brief Declaration of the udp_itor_t
    
*/


#ifndef __NEOIP_UDP_ITOR_CB_HPP__ 
#define __NEOIP_UDP_ITOR_CB_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class udp_itor_t;
class udp_event_t;

/** \brief the callback class for udp_itor_t
 */
class udp_itor_cb_t {
public:
	/** \brief callback notified by \ref udp_itor_t when a connection is established
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param udp_itor 	the udp_itor_t which notified this callback
	 * @param udp_event	The notified event
	 * @return true if the udp_itor_t is still valid after the callback
	 */
	virtual bool neoip_inet_udp_itor_event_cb(void *cb_userptr, udp_itor_t &cb_udp_itor
							, const udp_event_t &udp_event)
							throw() = 0;
	virtual ~udp_itor_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_ITOR_CB_HPP__  */




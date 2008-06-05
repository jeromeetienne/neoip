/*! \file
    \brief Declaration of the udp_client_t
    
*/


#ifndef __NEOIP_UDP_CLIENT_CB_HPP__ 
#define __NEOIP_UDP_CLIENT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class udp_client_t;
class udp_event_t;

/** \brief the callback class for udp_client_t
 */
class udp_client_cb_t {
public:
	/** \brief callback notified by \ref udp_client_t when to notify an event
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param udp_client 	the udp_client_t which notified this callback
	 * @param udp_event	notified event
	 * @return false if the udp_client_t has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_udp_client_event_cb(void *cb_userptr, udp_client_t &cb_udp_client
							, const udp_event_t &udp_event)	throw() = 0;
	virtual ~udp_client_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_CLIENT_CB_HPP__  */




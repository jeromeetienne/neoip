/*! \file
    \brief Declaration of the udp_resp_t
    
*/


#ifndef __NEOIP_UDP_RESP_CB_HPP__ 
#define __NEOIP_UDP_RESP_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class udp_resp_t;
class udp_event_t;

/** \brief the callback class for udp_resp_t
 */
class udp_resp_cb_t {
public:
	/** \brief callback notified by \ref udp_resp_t when a connection is established
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param udp_resp 	the udp_resp_t which notified this callback
	 * @param udp_event	it is ALWAYS udp_event_t::CNX_ESTABLISHED
	 * @return true if the udp_resp_t is still valid after the callback
	 */
	virtual bool neoip_inet_udp_resp_event_cb(void *cb_userptr, udp_resp_t &cb_udp_resp
							, const udp_event_t &udp_event)
							throw() = 0;
	virtual ~udp_resp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_RESP_CB_HPP__  */




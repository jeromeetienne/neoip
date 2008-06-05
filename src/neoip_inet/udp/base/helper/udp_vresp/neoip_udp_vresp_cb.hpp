/*! \file
    \brief Declaration of the udp_vresp_t
    
*/


#ifndef __NEOIP_UDP_VRESP_CB_HPP__ 
#define __NEOIP_UDP_VRESP_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class udp_vresp_t;
class udp_event_t;

/** \brief the callback class for udp_vresp_t
 */
class udp_vresp_cb_t {
public:
	/** \brief callback notified by \ref udp_vresp_t when a connection is established
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param udp_vresp 	the udp_vresp_t which notified this callback
	 * @param udp_event	it is ALWAYS udp_event_t::CNX_ESTABLISHED
	 * @return true if the udp_vresp_t is still valid after the callback
	 */
	virtual bool neoip_inet_udp_vresp_event_cb(void *cb_userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event)	throw() = 0;
	virtual ~udp_vresp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_VRESP_CB_HPP__  */




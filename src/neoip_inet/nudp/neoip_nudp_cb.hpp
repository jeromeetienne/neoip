/*! \file
    \brief Declaration of the udp_resp_t
    
*/


#ifndef __NEOIP_NUDP_CB_HPP__ 
#define __NEOIP_NUDP_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_pkt.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class nudp_t;

/** \brief the callback class for udp_resp_t
 */
class nudp_cb_t {
public:
	/** \brief callback notified when a nudp_t receive a packet
	 */
	virtual bool neoip_inet_nudp_event_cb(void *cb_userptr, nudp_t &cb_nudp, pkt_t &pkt
					, const ipport_addr_t &local_addr
					, const ipport_addr_t &remote_addr)	throw() = 0;
	virtual ~nudp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUDP_CB_HPP__  */




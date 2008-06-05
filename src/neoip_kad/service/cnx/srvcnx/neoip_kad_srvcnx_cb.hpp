/*! \file
    \brief Declaration of the udp_resp_t
    
*/


#ifndef __NEOIP_KAD_SRVCNX_CB_HPP__ 
#define __NEOIP_KAD_SRVCNX_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class kad_srvcnx_t;
class pkt_t;
class ipport_addr_t;

/** \brief the callback class for udp_resp_t
 */
class kad_srvcnx_cb_t {
public:
	/** \brief callback notified when a kad_srvcnx_t receive a packet
	 */
	virtual bool neoip_kad_srvcnx_cb(void *cb_userptr, kad_srvcnx_t &cb_kad_srvcnx, pkt_t &pkt
					, const ipport_addr_t &local_oaddr
					, const ipport_addr_t &remote_oaddr)	throw() = 0;
	virtual ~kad_srvcnx_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_SRVCNX_CB_HPP__  */




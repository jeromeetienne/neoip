/*! \file
    \brief Declaration of the udp_resp_t
    
*/


#ifndef __NEOIP_KAD_CLICNX_CB_HPP__ 
#define __NEOIP_KAD_CLICNX_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class kad_clicnx_t;
class kad_event_t;

/** \brief the callback class for kad_clicnx_t
 */
class kad_clicnx_cb_t {
public:
	/** \brief callback notified when a kad_clicnx_t receive a packet
	 */
	virtual bool neoip_kad_clicnx_cb(void *cb_userptr, kad_clicnx_t &cb_kad_clicnx
					, const kad_event_t &kad_event)	throw() = 0;
	virtual ~kad_clicnx_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_CLICNX_CB_HPP__  */




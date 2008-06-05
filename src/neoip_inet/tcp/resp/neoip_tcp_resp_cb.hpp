/*! \file
    \brief Declaration of the tcp_resp_t callback
    
*/


#ifndef __NEOIP_TCP_RESP_CB_HPP__ 
#define __NEOIP_TCP_RESP_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class tcp_resp_t;
class tcp_event_t;

/** \brief the callback class for tcp_resp_t
 */
class tcp_resp_cb_t {
public:
	/** \brief callback notified by \ref tcp_resp_t when a connection is established
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param tcp_resp 	the tcp_resp_t which notified this callback
	 * @param tcp_event	it is ALWAYS tcp_event_t::CNX_ESTABLISHED
	 * @return true if the tcp_resp_t is still valid after the callback
	 */
	virtual bool neoip_tcp_resp_event_cb(void *cb_userptr, tcp_resp_t &cb_tcp_resp
							, const tcp_event_t &tcp_event)	throw() = 0;
	// virtual destructor
	virtual ~tcp_resp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_RESP_CB_HPP__  */




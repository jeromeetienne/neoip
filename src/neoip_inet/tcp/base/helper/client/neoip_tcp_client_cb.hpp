/*! \file
    \brief Declaration of the tcp_client_t
    
*/


#ifndef __NEOIP_TCP_CLIENT_CB_HPP__ 
#define __NEOIP_TCP_CLIENT_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class tcp_client_t;
class tcp_event_t;

/** \brief the callback class for tcp_client_t
 */
class tcp_client_cb_t {
public:
	/** \brief callback notified by \ref tcp_client_t when to notify an event
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param tcp_client 	the tcp_client_t which notified this callback
	 * @param tcp_event	notified event
	 * @return true if the tcp_client_t is still valid after the callback
	 */
	virtual bool neoip_tcp_client_event_cb(void *cb_userptr, tcp_client_t &cb_tcp_client
							, const tcp_event_t &tcp_event)	throw() = 0;
	// virtual destructor
	virtual ~tcp_client_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_CLIENT_CB_HPP__  */




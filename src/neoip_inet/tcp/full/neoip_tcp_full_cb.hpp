/*! \file
    \brief Declaration of the tcp_full_t
    
*/


#ifndef __NEOIP_TCP_FULL_CB_HPP__ 
#define __NEOIP_TCP_FULL_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class tcp_full_t;
class tcp_event_t;

/** \brief the callback class for tcp_full_t
 */
class tcp_full_cb_t {
public:
	/** \brief callback notified by \ref tcp_full_t when to notify an event
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param tcp_full 	the tcp_full_t which notified this callback
	 * @param tcp_event	notified event
	 * @return true if the tcp_full_t is still valid after the callback
	 */
	virtual bool neoip_tcp_full_event_cb(void *cb_userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event)	throw() = 0;
	// virtual destructor
	virtual ~tcp_full_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_FULL_CB_HPP__  */




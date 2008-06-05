/*! \file
    \brief Declaration of the tcp_itor_t
    
*/


#ifndef __NEOIP_TCP_ITOR_CB_HPP__ 
#define __NEOIP_TCP_ITOR_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class tcp_itor_t;
class tcp_event_t;

/** \brief the callback class for tcp_itor_t
 */
class tcp_itor_cb_t {
public:
	/** \brief callback notified by \ref tcp_itor_t when a connection is established
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param tcp_itor 	the tcp_itor_t which notified this callback
	 * @param tcp_event	The notified event
	 * @return true if the tcp_itor_t is still valid after the callback
	 */
	virtual bool neoip_tcp_itor_event_cb(void *cb_userptr, tcp_itor_t &cb_tcp_itor
							, const tcp_event_t &tcp_event)	throw() = 0;
	//! virtual destructor
	virtual ~tcp_itor_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_ITOR_CB_HPP__  */




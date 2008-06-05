/*! \file
    \brief Declaration of the ntudp_client_t
    
*/


#ifndef __NEOIP_NTUDP_CLIENT_CB_HPP__ 
#define __NEOIP_NTUDP_CLIENT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_client_t;
class ntudp_event_t;

/** \brief the callback class for ntudp_client_t
 */
class ntudp_client_cb_t {
public:
	/** \brief callback notified by \ref ntudp_client_t when to notify an event
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param ntudp_client 	the ntudp_client_t which notified this callback
	 * @param ntudp_event	notified event
	 * @return false if the ntudp_client_t has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_ntudp_client_event_cb(void *cb_userptr, ntudp_client_t &cb_ntudp_client
							, const ntudp_event_t &ntudp_event)	throw() = 0;
	virtual ~ntudp_client_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_CLIENT_CB_HPP__  */




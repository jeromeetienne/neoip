/*! \file
    \brief Declaration of the ntudp_full_t
    
*/


#ifndef __NEOIP_NTUDP_FULL_CB_HPP__ 
#define __NEOIP_NTUDP_FULL_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_full_t;
class ntudp_event_t;

/** \brief the callback class for ntudp_full_t
 */
class ntudp_full_cb_t {
public:
	/** \brief callback notified by \ref ntudp_full_t when a connection is established
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param ntudp_full 	the ntudp_full_t which notified this callback
	 * @param ntudp_event	the ntudp_event_t describing the event
	 * @return false if the cb_ntudp_full has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_ntudp_full_event_cb(void *cb_userptr, ntudp_full_t &cb_ntudp_full
							, const ntudp_event_t &ntudp_event)	throw() = 0;
	virtual ~ntudp_full_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_FULL_CB_HPP__  */




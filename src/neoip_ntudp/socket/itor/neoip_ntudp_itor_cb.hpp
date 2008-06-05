/*! \file
    \brief Declaration of the ntudp_itor_t
    
*/


#ifndef __NEOIP_NTUDP_ITOR_CB_HPP__ 
#define __NEOIP_NTUDP_ITOR_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_itor_t;
class ntudp_event_t;

/** \brief the callback class for ntudp_itor_t
 */
class ntudp_itor_cb_t {
public:
	/** \brief callback notified by \ref ntudp_itor_t when it has an event to notify
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param ntudp_itor 	the ntudp_itor_t which notified this callback
	 * @param ntudp_event	the notified event
	 * @return false if the cb_ntudp_itor has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_ntudp_itor_event_cb(void *cb_userptr, ntudp_itor_t &cb_ntudp_itor
							, const ntudp_event_t &ntudp_event)	throw() = 0;
	virtual ~ntudp_itor_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_ITOR_CB_HPP__  */




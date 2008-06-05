/*! \file
    \brief Declaration of the ntudp_relpeer_t
    
*/


#ifndef __NEOIP_NTUDP_RELPEER_CB_HPP__ 
#define __NEOIP_NTUDP_RELPEER_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class ntudp_relpeer_t;

/** \brief the callback class for ntudp_relpeer_t
 */
class ntudp_relpeer_cb_t {
public:
	/** \brief callback notified by \ref ntudp_relpeer_t when to notify an event
	 * 
	 * @param userptr  		the userptr associated with this callback
	 * @param cb_ntudp_relpeer 	the ntudp_relpeer_t which notified this callback
	 * @return false if the ntudp_relpeer_t has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_ntudp_relpeer_cb(void *cb_userptr, ntudp_relpeer_t &cb_ntudp_relpeer) 
										throw() = 0;
	virtual ~ntudp_relpeer_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RELPEER_CB_HPP__  */




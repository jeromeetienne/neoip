/*! \file
    \brief Declaration of the ntudp_resp_t
    
*/


#ifndef __NEOIP_NTUDP_RESP_CB_HPP__ 
#define __NEOIP_NTUDP_RESP_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_resp_t;
class ntudp_event_t;

/** \brief the callback class for ntudp_resp_t
 */
class ntudp_resp_cb_t {
public:
	/** \brief callback notified by \ref ntudp_resp_t when a connection is established
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param ntudp_resp 	the ntudp_resp_t which notified this callback
	 * @param ntudp_event	it is ALWAYS ntudp_event_t::CNX_ESTABLISHED
	 * @return false if the cb_ntudp_resp has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_ntudp_resp_event_cb(void *cb_userptr, ntudp_resp_t &cb_ntudp_resp
							, const ntudp_event_t &ntudp_event)	throw() = 0;
	virtual ~ntudp_resp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RESP_CB_HPP__  */




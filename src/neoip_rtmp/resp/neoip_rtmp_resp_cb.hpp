/*! \file
    \brief Declaration of the rtmp_resp_t

*/


#ifndef __NEOIP_RTMP_RESP_CB_HPP__
#define __NEOIP_RTMP_RESP_CB_HPP__

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	rtmp_resp_t;
class	rtmp_event_t;

/** \brief the callback class for rtmp_resp_t
 */
class rtmp_resp_cb_t {
public:
	/** \brief callback notified by \ref rtmp_resp_t when to notify an event
	 *
	 * @return true if the rtmp_resp_t is still valid after the callback
	 */
	virtual bool neoip_rtmp_resp_cb(void *cb_userptr, rtmp_resp_t &cb_rtmp_resp
						, const rtmp_event_t &rtmp_event)	throw() = 0;
	// virtual destructor
	virtual ~rtmp_resp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_RESP_CB_HPP__  */




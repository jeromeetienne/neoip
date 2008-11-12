/*! \file
    \brief Declaration of the rtmp_full_t

*/


#ifndef __NEOIP_RTMP_FULL_CB_HPP__
#define __NEOIP_RTMP_FULL_CB_HPP__

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	rtmp_full_t;
class	rtmp_event_t;

/** \brief the callback class for rtmp_full_t
 */
class rtmp_full_cb_t {
public:
	/** \brief callback notified by \ref rtmp_full_t when to notify an event
	 *
	 * @return true if the rtmp_full_t is still valid after the callback
	 */
	virtual bool neoip_rtmp_full_cb(void *cb_userptr, rtmp_full_t &cb_rtmp_full
						, const rtmp_event_t &rtmp_event)	throw() = 0;
	// virtual destructor
	virtual ~rtmp_full_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_FULL_CB_HPP__  */




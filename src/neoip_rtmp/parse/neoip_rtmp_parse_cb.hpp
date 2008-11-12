/*! \file
    \brief Declaration of the rtmp_parse_t

*/


#ifndef __NEOIP_RTMP_PARSE_CB_HPP__
#define __NEOIP_RTMP_PARSE_CB_HPP__

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	rtmp_parse_t;
class	rtmp_event_t;

/** \brief the callback class for rtmp_parse_t
 */
class rtmp_parse_cb_t {
public:
	/** \brief callback notified by \ref rtmp_parse_t when to notify an event
	 *
	 * @return true if the rtmp_parse_t is still valid after the callback
	 */
	virtual bool neoip_rtmp_parse_cb(void *cb_userptr, rtmp_parse_t &cb_rtmp_parse
					, const rtmp_event_t &parse_event)	throw() = 0;
	// virtual destructor
	virtual ~rtmp_parse_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_PARSE_CB_HPP__  */




/*! \file
    \brief Declaration of the rtmp_cam_resp_t

*/


#ifndef __NEOIP_RTMP_CAM_RESP_CB_HPP__
#define __NEOIP_RTMP_CAM_RESP_CB_HPP__

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	rtmp_cam_resp_t;
class	rtmp_cam_full_t;

/** \brief the callback class for rtmp_cam_resp_t
 */
class rtmp_cam_resp_cb_t {
public:
	/** \brief callback notified by \ref rtmp_cam_resp_t when to notify an event
	 *
	 * @return true if the rtmp_cam_resp_t is still valid after the callback
	 */
	virtual bool neoip_rtmp_cam_resp_cb(void *cb_userptr, rtmp_cam_resp_t &cb_cam_resp
						, rtmp_cam_full_t *cb_cam_full)	throw() = 0;
	// virtual destructor
	virtual ~rtmp_cam_resp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_CAM_RESP_CB_HPP__  */




/*! \file
    \brief Header of the rtmp_cam_full_t

*/


#ifndef __NEOIP_RTMP_CAM_FULL_HPP__
#define __NEOIP_RTMP_CAM_FULL_HPP__
/* system include */
/* local include */
#include "neoip_rtmp_cam_full_cb.hpp"
#include "neoip_rtmp_full_cb.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_object_slotid.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_event_t;
class	rtmp_pkthd_t;

/** \brief class definition for rtmp_cam_full_t
 */
class rtmp_cam_full_t : NEOIP_COPY_CTOR_DENY, private rtmp_full_cb_t, public object_slotid_t {
private:
	/************** rtmp_full__t	****************************************/
	rtmp_full_t *	rtmp_full;
	bool		neoip_rtmp_full_cb(void *cb_userptr, rtmp_full_t &cb_rtmp_full
					, const rtmp_event_t &parse_event)	throw();

	/************** handle invoke	****************************************/
	bool		handle_invoke(const rtmp_event_t &rtmp_event)		throw();
	bool		handle_invoke_connect(const rtmp_pkthd_t &rtmp_pkthd
						, bytearray_t &amf0_body)	throw();
	bool		handle_invoke_createStream(const rtmp_pkthd_t &rtmp_pkthd
						, bytearray_t &amf0_body)	throw();

	/*************** callback stuff	***************************************/
	rtmp_cam_full_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const rtmp_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	rtmp_cam_full_t()		throw();
	~rtmp_cam_full_t()		throw();

	/*************** setup function	***************************************/
	rtmp_cam_full_t&set_callback(rtmp_cam_full_cb_t *callback, void *userptr)throw();
	rtmp_err_t	start(rtmp_full_t *rtmp_full, rtmp_cam_full_cb_t *callback
						, void *userptr)		throw();

	/*************** Action function	*******************************/
	rtmp_err_t	send(const void *data_ptr, size_t data_len) 	throw();
	rtmp_err_t	send(const pkt_t &pkt) 		throw()		{ return send(pkt.void_ptr(), pkt.length());	}
	rtmp_err_t	send(const datum_t &datum) 	throw()		{ return send(datum.void_ptr(), datum.length());}


	/*************** List of friend class	*******************************/
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_CAM_FULL_HPP__  */




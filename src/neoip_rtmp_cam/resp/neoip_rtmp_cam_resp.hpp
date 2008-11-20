/*! \file
    \brief Header of the \ref rtmp_cam_resp_t

*/


#ifndef __NEOIP_RTMP_CAM_RESP_HPP__
#define __NEOIP_RTMP_CAM_RESP_HPP__
/* system include */
/* local include */
#include "neoip_rtmp_cam_resp_wikidbg.hpp"
#include "neoip_rtmp_cam_resp_cb.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_cam_listener_t;
class	rtmp_cam_full_t;

/** \brief Handle the responder for a given uri/method attached to a given listener
 */
class rtmp_cam_resp_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<rtmp_cam_resp_t, rtmp_cam_resp_wikidbg_init>
			{
private:
	rtmp_cam_listener_t *	m_cam_listener;	//!< the rtmp_cam_listener_t
	http_uri_t		m_listen_uri;	//!< the uri, this responder is listening on

	/*************** callback stuff	***************************************/
	rtmp_cam_resp_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(rtmp_cam_full_t *rtmp_cam_full)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	rtmp_cam_resp_t()		throw();
	~rtmp_cam_resp_t()		throw();

	/*************** Setup function	***************************************/
	rtmp_err_t	start(rtmp_cam_listener_t *m_cam_listener, const http_uri_t &m_listen_uri
				, rtmp_cam_resp_cb_t *callback, void *userptr)	throw();

	/*************** Query function	***************************************/
	const http_uri_t &	listen_uri()	const throw()	{ return m_listen_uri;	}
	bool			may_handle(const http_uri_t &req_uri)	const throw();

	/*************** Action function	*******************************/
	void			notify_new_cnx(rtmp_cam_full_t *cam_full) throw();

	/*************** list of friend class	*******************************/
	friend class	rtmp_cam_resp_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_CAM_RESP_HPP__  */











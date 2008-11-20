/*! \file
    \brief Header of the bt_scasti_rtmp_t

*/


#ifndef __NEOIP_BT_SCASTI_RTMP_HPP__
#define __NEOIP_BT_SCASTI_RTMP_HPP__
/* system include */
/* local include */
#include "neoip_bt_scasti_rtmp_wikidbg.hpp"
#include "neoip_bt_scasti_rtmp_profile.hpp"
#include "neoip_bt_scasti_cb.hpp"
#include "neoip_bt_scasti_mod_type.hpp"
#include "neoip_bt_scasti_vapi.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_rtmp_cam_resp_cb.hpp"
#include "neoip_rtmp_cam_full_cb.hpp"
#include "neoip_bt_io_write_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_mfile_t;
class	bt_scasti_mod_vapi_t;
class	bt_scasti_mod_profile_t;
class	bt_io_vapi_t;
class	pkt_t;
class	rtmp_cam_listener_t;

/** \brief class definition for bt_scasti_rtmp_t
 */
class bt_scasti_rtmp_t : NEOIP_COPY_CTOR_DENY, public bt_scasti_vapi_t
		, private rtmp_cam_resp_cb_t
		, private rtmp_cam_full_cb_t
		, private bt_io_write_cb_t
		, private wikidbg_obj_t<bt_scasti_rtmp_t, bt_scasti_rtmp_wikidbg_init>  {
private:
	http_uri_t		m_scasti_uri;	//!< the source of the http stream
	bt_io_vapi_t *		m_io_vapi;	//!< the bt_io_vapi_t
	file_size_t		m_cur_offset;	//!< the amount of byte already written
	bt_scasti_rtmp_profile_t profile;	//!< the profile to use for this bt_scasti_rtmp_t

	uint32_t		prev_tagsize;	//!< the flv_taghd_t::prev_tagsize (for flv file writing)
	delay_t			prev_timestamp;	//!< the flv_taghd_t::prev_timestamp (for flv file writing)

	/*************** bt_scasti_mod_vapi_t	*******************************/
	bt_scasti_mod_vapi_t *	m_mod_vapi;	//!< pointer on the bt_scasti_mod_vapi_t
	bt_err_t		mod_vapi_ctor(const bt_scasti_mod_type_t &mod_type
					, const bt_scasti_mod_profile_t &mod_profile)	throw();
	bool			mod_vapi_notify_callback(const bt_scasti_event_t &event)throw();

	/*************** rtmp_cam_resp_t	*******************************/
	rtmp_cam_resp_t *	rtmp_cam_resp;
	bool			neoip_rtmp_cam_resp_cb(void *cb_userptr, rtmp_cam_resp_t &cb_cam_resp
					, rtmp_cam_full_t *cb_cam_full)		throw();
	/*************** rtmp_cam_full_t	*******************************/
	rtmp_cam_full_t *	rtmp_cam_full;
	bool			neoip_rtmp_cam_full_cb(void *cb_userptr, rtmp_cam_full_t &cb_cam_full
					, const rtmp_event_t &rtmp_event)	throw();
	bool			handle_recved_data(pkt_t &pkt)			throw();

	/*************** bt_io_write_t	***************************************/
	bt_io_write_t *		bt_io_write;
	bool 			neoip_bt_io_write_cb(void *cb_userptr, bt_io_write_t &cb_io_write
						, const bt_err_t &bt_err)	throw();

	/*************** callback stuff	***************************************/
	bt_scasti_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback_failed(const bt_err_t &bt_err)		throw();
	bool			notify_callback(const bt_scasti_event_t &scasti_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_scasti_rtmp_t() 		throw();
	~bt_scasti_rtmp_t()		throw();

	/*************** setup function	***************************************/
	bt_scasti_rtmp_t &	set_profile(const bt_scasti_rtmp_profile_t &profile)	throw();
	bt_err_t		start(const http_uri_t &m_scasti_uri, bt_io_vapi_t *m_io_vapi
					, const bt_scasti_mod_type_t &mod_type
					, rtmp_cam_listener_t * rtmp_cam_listener
					, bt_scasti_cb_t *callback, void *userptr) 	throw();

	/*************** query function	***************************************/
	const file_size_t &	cur_offset()	const throw()	{ return m_cur_offset;	}
	bt_scasti_mod_vapi_t *	mod_vapi()	const throw()	{ return m_mod_vapi;	}

	/*************** List of friend class	*******************************/
	friend class	bt_scasti_rtmp_wikidbg_t;
	friend class	bt_scasti_mod_flv_t;
	friend class	bt_scasti_mod_raw_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SCASTI_RTMP_HPP__  */




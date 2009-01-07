/*! \file
    \brief Class to handle the bt_scasti_rtmp_t

\par Brief Description
bt_scasti_rtmp_t reads data from a rtmp connection and write it in
a bt_io_pfile_t as a "circular buffer"
- it starts writing at the offset 0
- and will warp around the buffer size (i.e. bt_mfile.totfile_size) when it is
  reached.

\par About chunk notification
- when a chunk is notified, it is already written to disk
  - so it can be read if needed
- the bt_scasti_rtmp_t::cur_offset() is the one at which the chunk starts
  - aka the length of the chunk is updated AFTER the callback notification

\par About the circularidx
- bt_scasti_rtmp_t do not handle any of the circularidx, it is handled by
  the bt_io_pfile_t

*/

/* system include */
/* local include */
#include "neoip_bt_scasti_rtmp.hpp"
#include "neoip_bt_scasti_event.hpp"
#include "neoip_bt_scasti_mod_vapi.hpp"
#include "neoip_bt_scasti_mod_raw.hpp"
#include "neoip_bt_scasti_mod_flv.hpp"
#include "neoip_bt_io_write.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_bt_io_pfile.hpp"
#include "neoip_rtmp_cam.hpp"
#include "neoip_flv_amf0.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

#include "neoip_rtmp.hpp"
#include "neoip_socket_full.hpp"	// TODO to fix - ugly kludge to get socket_full in rtmp_full_t
#include "neoip_rtmp_net.hpp"		// TODO to fix - ugly kludge to get socket_full in rtmp_full_t

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
bt_scasti_rtmp_t::bt_scasti_rtmp_t()	throw()
{
	// zero some fields
	m_io_vapi	= NULL;
	m_mod_vapi	= NULL;
	rtmp_cam_resp	= NULL;
	rtmp_cam_full	= NULL;
	bt_io_write	= NULL;
	m_cur_offset	= 0;
}

/** \brief Destructor
 */
bt_scasti_rtmp_t::~bt_scasti_rtmp_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the bt_scasti_mod_vapi_t if needed
	nipmem_zdelete	m_mod_vapi;
	// delete the bt_io_write_t if needed
	nipmem_zdelete	bt_io_write;
	// delete rtmp_cam_resp_t if needed
	nipmem_zdelete	rtmp_cam_resp;
	// delete rtmp_cam_full_t if needed
	nipmem_zdelete	rtmp_cam_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_scasti_rtmp_t &	bt_scasti_rtmp_t::set_profile(const bt_scasti_rtmp_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_scasti_rtmp_t::start(const http_uri_t &m_scasti_uri, bt_io_vapi_t *m_io_vapi
				, const bt_scasti_mod_type_t &mod_type
				, rtmp_cam_listener_t * rtmp_cam_listener
				, bt_scasti_cb_t *callback, void *userptr) 	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// copy the parameters
	this->m_scasti_uri	= m_scasti_uri;
	this->m_io_vapi		= m_io_vapi;
	this->callback		= callback;
	this->userptr		= userptr;

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/bt_scasti_rtmp_" + OSTREAMSTR(this));

	// start the rtmp_cam_resp_t
	rtmp_err_t	rtmp_err;
	rtmp_cam_resp	= nipmem_new rtmp_cam_resp_t();
	rtmp_err	= rtmp_cam_resp->start(rtmp_cam_listener, m_scasti_uri, this, NULL);
	if( rtmp_err.failed() )	return bt_err_t(bt_err_t::ERROR, rtmp_err.to_string() );

	// start the bt_scasti_mod_vapi_t
	bt_err_t	bt_err;
	bt_err		= mod_vapi_ctor(mod_type, profile.mod());
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_scasti_mod_vapi_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Construct the bt_scasti_mod_vapi_t based on the bt_scasti_mod_type_t
 *
 * - TODO why this weird ctor here ?
 * - why mod_vapi has no callback ?
 */
bt_err_t	bt_scasti_rtmp_t::mod_vapi_ctor(const bt_scasti_mod_type_t &mod_type
						, const bt_scasti_mod_profile_t &mod_profile)	throw()
{
	bt_err_t		bt_err	= bt_err_t::OK;
	bt_scasti_mod_raw_t *	mod_raw;
	bt_scasti_mod_flv_t *	mod_flv;
	// construct the bt_scasti_mod_vapi_t depending on the bt_scasti_mod_type_t
	switch( mod_type.get_value() ){
	case bt_scasti_mod_type_t::RAW:	mod_raw		= nipmem_new bt_scasti_mod_raw_t();
					m_mod_vapi	= mod_raw;
					bt_err		= mod_raw->profile(mod_profile.raw())
								.start(this);
					break;
	case bt_scasti_mod_type_t::FLV:	mod_flv		= nipmem_new bt_scasti_mod_flv_t();
					m_mod_vapi	= mod_flv;
					bt_err		= mod_flv->start(this);
					break;
	default:			DBG_ASSERT( 0 );
	}
	// return noerror
	return bt_err_t::OK;
}

/** \brief Called by bt_scasti_mod_vapi_t to notify event to the caller
 */
bool	bt_scasti_rtmp_t::mod_vapi_notify_callback(const bt_scasti_event_t &event)	throw()
{
	// TODO to remove
	// just forward the bt_scasti_event_t to the caller
	return notify_callback(event);
}



/** \brief callback notified by \ref bt_scasti_mod_vapi_t
 */
bool	bt_scasti_rtmp_t::neoip_bt_scasti_mod_cb(void *cb_userptr, bt_scasti_mod_vapi_t &cb_mod_vapi
				, const bt_scasti_event_t &scasti_event)	throw()
{
	// just forward the bt_scasti_event_t to the caller
	return notify_callback(scasti_event);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			flv building
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief push the flv_header in m_buffer
 */
void	bt_scasti_rtmp_t::push_flv_header()	throw()
{
	// sanity check - prev_timestamp MUST be null
	DBG_ASSERT( prev_timestamp.is_null() );
	// log to debug
	KLOG_ERR("push flv header");
	// init the local data to rebuild the flv
	prev_tagsize	= 0;
	prev_timestamp	= delay_t::from_sec(0);

	// build the flv_tophd_t
	m_buffer	<< flv_tophd_t().version(1).flag(flv_tophd_flag_t::VIDEO | flv_tophd_flag_t::AUDIO);


	// get the values for metadata from rtmp_cam_full_t->connect_uri();
	const http_uri_t &	connect_uri	= rtmp_cam_full->connect_uri();
	const strvar_db_t &	uri_var		= connect_uri.var();
	uint32_t		video_w		= string_t::to_uint32(uri_var.get_first_value("video_w"	, "320"));
	uint32_t		video_h		= string_t::to_uint32(uri_var.get_first_value("video_h"	, "240"));
	uint32_t		video_fps	= string_t::to_uint32(uri_var.get_first_value("video_fps", "15"));
	// build the amf0 metadata for the flv
	bytearray_t	amf0_resp;
	amf0_build_t::to_amf0(dvar_str_t("onMetaData")	, amf0_resp);
	amf0_build_t::to_amf0(dvar_map_t()
			.insert("width"		, dvar_dbl_t(video_w)	)
			.insert("height"	, dvar_dbl_t(video_h)	)
			.insert("framerate"	, dvar_dbl_t(video_fps)	)
							, amf0_resp);
	datum_t		rtmp_pktbd	= amf0_resp.to_datum();

	// build the flv_taghd_t
	flv_taghd_t	flv_taghd;
	flv_taghd.prevtag_size	( prev_tagsize );
	flv_taghd.type		( flv_tagtype_t::META );
	flv_taghd.body_length	( rtmp_pktbd.length() );
	flv_taghd.padding	( 0 );
	flv_taghd.timestamp	( prev_timestamp );
	// push the flv_taghd_t in the pkt_t
	m_buffer	<< flv_taghd;
	// push the rtmp_pktbd
	m_buffer.append(rtmp_pktbd);
	// update prev_tagsize/prev_timestamp
	prev_tagsize	+= rtmp_pktbd.length() + 11;
	prev_timestamp	+= delay_t::from_msec(1);
}

/** \brief push the flv audio/video in m_buffer
 */
void	bt_scasti_rtmp_t::push_flv_avideo(const rtmp_event_t &rtmp_event)	throw()
{
	// sanity check - at this point, rtmp_event_t MUST be a rtmp_event_t:PACKET
	DBG_ASSERT( rtmp_event.is_packet() );
	// get the packet from the rtmp_event_t
	datum_t		rtmp_pktbd;
	rtmp_pkthd_t	rtmp_pkthd	= rtmp_event.get_packet(&rtmp_pktbd);
	DBG_ASSERT( rtmp_pkthd.type() == rtmp_type_t::AUDIO || rtmp_pkthd.type() == rtmp_type_t::VIDEO );
	// build the flv_taghd_t
	flv_taghd_t	flv_taghd;
	flv_taghd.prevtag_size	( prev_tagsize );
	if( rtmp_pkthd.type() == rtmp_type_t::VIDEO )		flv_taghd.type(flv_tagtype_t::VIDEO);
	else if( rtmp_pkthd.type() == rtmp_type_t::AUDIO )	flv_taghd.type(flv_tagtype_t::AUDIO);
	else	DBG_ASSERT(0);
	flv_taghd.body_length	( rtmp_pktbd.length() );
	flv_taghd.padding	( 0 );
	flv_taghd.timestamp	( prev_timestamp );
	// push the flv_taghd_t in the pkt_t
	m_buffer	<< flv_taghd;

	// push the rtmp_pktbd
	m_buffer.append(rtmp_pktbd);

	// update prev_tagsize/prev_timestamp
	prev_tagsize	+= rtmp_pktbd.length() + 11;
	prev_timestamp	+= rtmp_pkthd.timestamp();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_cam_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_resp_t when to notify an event
 */
bool	bt_scasti_rtmp_t::neoip_rtmp_cam_resp_cb(void *cb_userptr, rtmp_cam_resp_t &cb_cam_resp
					, rtmp_cam_full_t *cb_cam_full)	throw()
{
	// sanity check - rtmp_cal_full MUST be null at this point
	// - rtmp_cam_resp_t is deleted just after the first spawn
	DBG_ASSERT( rtmp_cam_full == NULL );
	// log to debug
	KLOG_ERR("enter");
	// set the callback on the local object
	// - it is already started by rtmp_cam_resp_t
	rtmp_cam_full	= cb_cam_full;
	rtmp_cam_full->set_callback(this, NULL);

	// delete rtmp_cam_resp_t
	nipmem_zdelete	rtmp_cam_resp;
	// return donkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_cam_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_resp_t when to notify an event
 */
bool	bt_scasti_rtmp_t::neoip_rtmp_cam_full_cb(void *cb_userptr, rtmp_cam_full_t &cb_cam_full
				, const rtmp_event_t &rtmp_event)	throw()
{
	// sanity check - rtmp_event_t MUST be is_cam_full_ok()
	DBG_ASSERT( rtmp_event.is_cam_full_ok() );
	// log to debug
	KLOG_ERR("enter event=" << rtmp_event);

	// if rtmp_event.is_fatal(), delete the rtmp_cam_full_t
	if( rtmp_event.is_fatal() )	return notify_callback_failed( bt_err_t(bt_err_t::ERROR, rtmp_event.to_string()));

	// sanity check - at this point, rtmp_event_t MUST be a rtmp_event_t:PACKET
	DBG_ASSERT( rtmp_event.is_packet() );

	// push flv header IIF first rtmp_event_t::PACKET
	if( prev_timestamp.is_null() )	push_flv_header();

	// push the packet for this event
	push_flv_avideo(rtmp_event);

	// check that m_buffer is < profile.rcvdata_maxlen()
	if( m_buffer.length() >= profile.rcvdata_maxlen() ){
		bt_err_t bt_err(bt_err_t::ERROR, "received data greater than "+ OSTREAMSTR(profile.rcvdata_maxlen()));
		return notify_callback_failed(bt_err);
	}

	// launch a bt_io_write_t if needed
	launch_write_if_needed();

	// return tokeep
	return true;
}

/** \brief Handle the received packet
 *
 * @return a tokeep for the socket_full_t
 */
void	bt_scasti_rtmp_t::launch_write_if_needed()	throw()
{
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);

	// if bt_io_write_t is in progress, return now
	if( bt_io_write )	return;
	// if m_buffer.empty(), return now
	if( m_buffer.empty() )	return;

	// notify the data to the bt_scasti_mod_vapi_t
	m_mod_vapi->notify_data(m_buffer.to_datum(datum_t::NOCOPY));

	// sanity check - no bt_io_write_t MUST be inprogress
	DBG_ASSERT( bt_io_write == NULL );

	// launcht the bt_io_write_t
	// - a bt_io_write_t NEVER fails on launch
	// - bt_io_vapi_t will handle the circularidx if needed
	file_range_t	file_range(cur_offset(), cur_offset() + m_buffer.length()-1);
	bt_io_write	= m_io_vapi->write_ctor(file_range, m_buffer.to_datum(datum_t::NOCOPY)
							, this, NULL);
	// empty m_buffer as it is now in the bt_io_write_t
	m_buffer.tail_free(m_buffer.length());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        bt_io_write_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	bt_scasti_rtmp_t::neoip_bt_io_write_cb(void *cb_userptr, bt_io_write_t &cb_io_write
						, const bt_err_t &bt_err)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err << " written_len=" << bt_io_write->written_len());

	// if the bt_io_write_t failed, notify an error
	if( bt_err.failed() )	return notify_callback_failed(bt_err);

	// build the bt_scasti_event_t::CHUNK_AVAIL
	bt_scasti_event_t scasti_event;
	scasti_event	= bt_scasti_event_t::build_chunk_avail(bt_io_write->written_len());
	bool	tokeep	= notify_callback(scasti_event);
	if( !tokeep )	return false;

	// update the m_cur_offset
	// - it MUST be done AFTER the callback notification
	// - thus the notified function may read cur_offset
	m_cur_offset	+= bt_io_write->written_len();

	// autodelete this sched_block and return dontkeep
	nipmem_zdelete	bt_io_write;

	// launch a bt_io_write_t if needed
	launch_write_if_needed();

	// return dontkeep - as the bt_io_write_t as just been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback in case of a faillure
 */
bool	bt_scasti_rtmp_t::notify_callback_failed(const bt_err_t &bt_err)		throw()
{
	// build the bt_scasti_event_t
	bt_scasti_event_t scasti_event	= bt_scasti_event_t::build_error(bt_err);
	// forward to notify_callback
	return notify_callback(scasti_event);
}

/** \brief notify the callback with the bt_scasti_event_t
 */
bool	bt_scasti_rtmp_t::notify_callback(const bt_scasti_event_t &scasti_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_scasti_cb(userptr, *this, scasti_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END






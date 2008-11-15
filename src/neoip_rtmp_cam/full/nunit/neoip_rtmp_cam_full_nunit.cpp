/*! \file
    \brief Definition of the unit test for the \ref rtmp_resp_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_cam_full_nunit.hpp"
#include "neoip_rtmp_cam.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_file.hpp"	// include to write the .flv file
#include "neoip_flv.hpp"	// include to write the .flv file
#include "neoip_rtmp.hpp"	// include to write the .flv file

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
rtmp_cam_full_testclass_t::rtmp_cam_full_testclass_t()	throw()
{
	// zero some field
	cam_listener	= NULL;
	cam_resp	= NULL;
	cam_full	= NULL;
}

/** \brief Destructor
 */
rtmp_cam_full_testclass_t::~rtmp_cam_full_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	rtmp_cam_full_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	rtmp_cam_full_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete variables if needed
	nipmem_zdelete	cam_full;
	nipmem_zdelete	cam_resp;
	nipmem_zdelete	cam_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	rtmp_cam_full_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	rtmp_err_t	rtmp_err;
	// build the resp_arg
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
					.listen_addr("tcp://0.0.0.0:1935");
	// start the rtmp_cam_listener_t
	cam_listener	= nipmem_new rtmp_cam_listener_t();
	rtmp_err	= cam_listener->start(resp_arg);
	if( rtmp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, rtmp_err.to_string() );

	// start the rtmp_cam_resp_t
	cam_resp	= nipmem_new rtmp_cam_resp_t();
	rtmp_err	= cam_resp->start(cam_listener, "rtmp://127.0.0.1/live", this, NULL);
	if( rtmp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, rtmp_err.to_string() );

#if 1	// handle the flv file writing
	// create the flv file to output
	bytearray_t	bytearray;
	bytearray	<< flv_tophd_t().version(1).flag(flv_tophd_flag_t::VIDEO | flv_tophd_flag_t::AUDIO);
	file_sio_t::writeall("/tmp/output.flv", bytearray.to_datum());
	prev_tagsize	= 0;
	prev_timestamp	= delay_t::from_sec(0);
#endif

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_cam_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_resp_t when to notify an event
 */
bool	rtmp_cam_full_testclass_t::neoip_rtmp_cam_resp_cb(void *cb_userptr, rtmp_cam_resp_t &cb_cam_resp
					, rtmp_cam_full_t *cb_cam_full)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// if there is already a rtmp_cam_full_t initialized, delete this one immediatly
	if( cam_full ){
		// log to debug
		KLOG_ERR("one rtmp_cam_full_t is already handled. dropping new one");
		nipmem_zdelete	cb_cam_full;
		return true;
	}
	// set the callback on the local object
	// - it is already started by rtmp_cam_resp_t
	cam_full	= cb_cam_full;
	cam_full->set_callback(this, NULL);

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_resp_t when to notify an event
 */
bool	rtmp_cam_full_testclass_t::neoip_rtmp_cam_full_cb(void *cb_userptr, rtmp_cam_full_t &cb_cam_full
				, const rtmp_event_t &rtmp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << rtmp_event);

	// if rtmp_event.is_fatal(), delete the rtmp_cam_full_t
	if( rtmp_event.is_fatal() ){
		// log to debug
		KLOG_ERR("delete rtmp_full_webcam_t due to " << rtmp_event);
		// delete rtmp_cam_full_t
		nipmem_zdelete	cam_full;
		// return dontkeep as rtmp_cam_full_t just got deleted
		return false;
	}

	// sanity check - at this point, rtmp_event_t MUST be a rtmp_event_t:PACKET
	DBG_ASSERT( rtmp_event.is_packet() );


#if 1	// handle the flv file writing
	// get the packet from the event
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
	// write this data in the file
	bytearray_t	bytearray;
	bytearray	<< flv_taghd;
	file_mode_t	file_mode	= file_mode_t::WRITE | file_mode_t::APPEND;
	file_sio_t::writeall("/tmp/output.flv", bytearray.to_datum()	, file_perm_t::FILE_DFL, file_mode);
	file_sio_t::writeall("/tmp/output.flv", rtmp_pktbd		, file_perm_t::FILE_DFL, file_mode);
	// update prev_tagsize
	prev_tagsize	+= rtmp_pktbd.length() + 11;
	prev_timestamp	+= rtmp_pkthd.timestamp();
#endif
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END


/*! \file
    \brief Class to handle the rtmp_full_t


*/

/* system include */
/* local include */
#include "neoip_rtmp_cam_full.hpp"
#include "neoip_rtmp.hpp"
#include "neoip_flv_amf0.hpp"
#include "neoip_dvar.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
rtmp_cam_full_t::rtmp_cam_full_t()	throw()
{
	// zero some fields
	rtmp_full	= NULL;
}

/** \brief Destructor
 */
rtmp_cam_full_t::~rtmp_cam_full_t()	throw()
{
	// delete the rtmp_full_t if needed
	nipmem_zdelete	rtmp_full;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Set the callback parameter
 */
rtmp_cam_full_t &rtmp_cam_full_t::set_callback(rtmp_cam_full_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// return the object itself
	return *this;
}
/** \brief Start the operation
 */
rtmp_err_t	rtmp_cam_full_t::start(rtmp_full_t *p_rtmp_full
		, rtmp_cam_full_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// set up the socket_full_t
	this->rtmp_full	= p_rtmp_full;
	this->rtmp_full->start(this, NULL);
	// return no error
	return rtmp_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     action function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Send data thru the rtmp_cam_full_t
 */
rtmp_err_t	rtmp_cam_full_t::send(const void *data_ptr, size_t data_len) 	throw()
{
	return rtmp_full->send(data_ptr, data_len);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     rtmp_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_full_t to provide event
 */
bool	rtmp_cam_full_t::neoip_rtmp_full_cb(void *userptr, rtmp_full_t &cb_rtmp_full
					, const rtmp_event_t &rtmp_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << rtmp_event);
	// sanity check - rtmp_event_t MUST be is_full_ok()
	DBG_ASSERT( rtmp_event.is_full_ok() );

	// if rtmp_event.is_fatal(), simply forward the event to the caller
	if( rtmp_event.is_fatal() )	return notify_callback(rtmp_event);

	// sanity check - at this point, rtmp_event_t MUST be a rtmp_event_t:PACKET
	DBG_ASSERT( rtmp_event.is_packet() );

	// get the packet from the event
	rtmp_pkthd_t	rtmp_pkthd	= rtmp_event.get_packet(NULL);

	// if rtmp_pkdhd.type() is AUDIO or VIDEO, simply forward to the caller
	switch( rtmp_pkthd.type().get_value() ){
	case rtmp_type_t::AUDIO:	// fall thru
	case rtmp_type_t::VIDEO:	return notify_callback(rtmp_event);
	case rtmp_type_t::INVOKE:	return handle_invoke(rtmp_event);
	default:	// simply ignore any other rtmp_type_t
			// - TODO fix this... this is dirty
			break;
	}

	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     handle rtmp_type_t::INVOKE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle rtmp_event_t for rtmp_type_t::INVOKE
 */
bool	rtmp_cam_full_t::handle_invoke(const rtmp_event_t &rtmp_event)	throw()
{
	// get the packet from the event
	datum_t		pktbody;
	rtmp_pkthd_t	rtmp_pkthd	= rtmp_event.get_packet(&pktbody);

	// log to debug
	KLOG_ERR("rtmp_pkthd="	<< rtmp_pkthd);
	KLOG_ERR("pktbody="	<< pktbody);

	// sanity check - at this point, rtmp_pkthd_t MUST BE rtmp_type_t::INVOKE
	DBG_ASSERT( rtmp_pkthd.type() == rtmp_type_t::INVOKE );

	// parse the packet body
	bytearray_t	amf0_body(pktbody);
	dvar_t	dvar	= amf0_parse_t::parser(amf0_body);
	if( dvar.str().get() == "connect" ){
		return handle_invoke_connect(rtmp_pkthd, amf0_body);
	}else if( dvar.str().get() == "createStream" ){
		return handle_invoke_createStream(rtmp_pkthd, amf0_body);
	}else{
		KLOG_ERR("received INVOKE '" << dvar.str() << "' but not handled");
	}

	// return tokeep
	return true;
}

/** \brief handle rtmp_event_t for rtmp_type_t::INVOKE "connect"
 */
bool	rtmp_cam_full_t::handle_invoke_connect(const rtmp_pkthd_t &rtmp_pkthd
						, bytearray_t &amf0_body)	throw()
{
	// sanity check - at this point, rtmp_pkthd_t MUST BE rtmp_type_t::INVOKE
	DBG_ASSERT( rtmp_pkthd.type() == rtmp_type_t::INVOKE );

	// get the parameter for the invoke("connect");
	dvar_t	param0	= amf0_parse_t::parser(amf0_body);
	dvar_t	param1	= amf0_parse_t::parser(amf0_body);

	// build the amf0_resp
	bytearray_t	amf0_resp;
	// build the INVOKE "connect" response
	amf0_build_t::to_amf0(dvar_str_t("_result")	, amf0_resp);
	amf0_build_t::to_amf0(param0			, amf0_resp);
	amf0_build_t::to_amf0(dvar_nil_t()		, amf0_resp);
	amf0_build_t::to_amf0(dvar_map_t()
			.insert("level"		, dvar_str_t("status"))
			.insert("description"	, dvar_str_t("Connection succeeded."))
			.insert("code"		, dvar_str_t("NetConnection.Connect.Success"))
							, amf0_resp);
	bytearray_t	data_resp;
	rtmp_build_t::serialize(rtmp_pkthd, amf0_resp, data_resp);
	// send the reply to the client
	rtmp_err_t	rtmp_err;
	rtmp_err	= rtmp_full->send(data_resp.to_datum());
	DBG_ASSERT(rtmp_err.succeed());

	// get the connect_uri from the connect parameter
	DBG_ASSERT(param1.type().is_map());
	http_uri_t	connect_uri	= param1.map()["tcUrl"].str().get();
	KLOG_ERR("tcUrl="<< connect_uri);

	// build and notify a rtmp_event_t::CONNECTED
	rtmp_event_t	rtmp_event;
	rtmp_event	= rtmp_event_t::build_connected(connect_uri);
	bool	tokeep	= notify_callback(rtmp_event);
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

/** \brief handle rtmp_event_t for rtmp_type_t::INVOKE "createStream"
 */
bool	rtmp_cam_full_t::handle_invoke_createStream(const rtmp_pkthd_t &rtmp_pkthd
						, bytearray_t &amf0_body)	throw()
{
	// sanity check - at this point, rtmp_pkthd_t MUST BE rtmp_type_t::INVOKE
	DBG_ASSERT( rtmp_pkthd.type() == rtmp_type_t::INVOKE );

	// get the parameter for the invoke("createStream");
	dvar_t	param0	= amf0_parse_t::parser(amf0_body);
	dvar_t	param1	= amf0_parse_t::parser(amf0_body);
	// build the amf0_resp
	bytearray_t	amf0_resp;
	// build the INVOKE "createStream" response
	amf0_build_t::to_amf0(dvar_str_t("_result")	, amf0_resp);
	amf0_build_t::to_amf0(param0			, amf0_resp);
	amf0_build_t::to_amf0(dvar_nil_t()		, amf0_resp);
	amf0_build_t::to_amf0(dvar_dbl_t(1)		, amf0_resp);
	bytearray_t	data_resp;
	rtmp_build_t::serialize(rtmp_pkthd, amf0_resp, data_resp);
	// send the reply to the client
	rtmp_err_t	rtmp_err;
	rtmp_err	= rtmp_full->send(data_resp.to_datum());
	DBG_ASSERT(rtmp_err.succeed());

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool rtmp_cam_full_t::notify_callback(const rtmp_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_rtmp_cam_full_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END






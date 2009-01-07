/*! \file
    \brief Class to handle the rtmp_full_t


*/

/* system include */
/* local include */
#include "neoip_rtmp_cam_full.hpp"
#include "neoip_rtmp.hpp"
#include "neoip_rtmp_ping_type.hpp"
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
//                     rtmp packet building
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Build a rtmp packet serverbw
 */
datum_t		rtmp_cam_full_t::build_rtmp_serverbw(uint32_t rate)	throw()
{
	rtmp_pkthd_t	rtmp_pkthd;
	// build the header
	rtmp_pkthd.channel_id	( 0x02 );
	rtmp_pkthd.timestamp	( delay_t::from_msec(0));
	rtmp_pkthd.type		( rtmp_type_t::SERVERBW );
	rtmp_pkthd.stream_id	( 0 );
	// build the body (not in AMF0)
	bytearray_t	data_body;
	data_body << 	rate;		// this one is interpreter as a signed integer
	data_body <<	uint8_t(2);	// i dunno what this 2 mean. but i saw it from other servers
	// serialize the packet
	bytearray_t	bytearray;
	rtmp_build_t::serialize(rtmp_pkthd, data_body, bytearray);
	// return the datum_t describing the packet
	return bytearray.to_datum();
}

/** \brief Build a rtmp packet clientbw
 */
datum_t		rtmp_cam_full_t::build_rtmp_clientbw(uint32_t rate)	throw()
{
	rtmp_pkthd_t	rtmp_pkthd;
	// build the header
	rtmp_pkthd.channel_id	( 0x02 );
	rtmp_pkthd.timestamp	( delay_t::from_msec(0) );
	rtmp_pkthd.type		( rtmp_type_t::CLIENTBW );
	rtmp_pkthd.stream_id	( 0 );
	// build the body (not in AMF0)
	bytearray_t	data_body;
	data_body << 	rate;
	// serialize the packet
	bytearray_t	bytearray;
	rtmp_build_t::serialize(rtmp_pkthd, data_body, bytearray);
	// return the datum_t describing the packet
	return bytearray.to_datum();
}

/** \brief Build a rtmp packet onBWDone (with the rtmp_pkthd_t of the invoke::connect)
 */
datum_t		rtmp_cam_full_t::build_rtmp_onBWDone(const rtmp_pkthd_t &rtmp_pkthd)	throw()
{
	// From rtmp-decoded.pdf
	// Bandwidth Checking Message
	// "After a successful connection is made, and the Connection.Succeeded message is
	//  turned, the server sends this message, which sets a callback for an optionally
	//  used method do bandwidth checking. This sets it to the default of “undefined”."
	// build the amf0_resp
	bytearray_t	amf0_body;
	// build the INVOKE "connect" response
	amf0_build_t::to_amf0(dvar_str_t("onBWDone")	, amf0_body);
	amf0_build_t::to_amf0(dvar_dbl_t(0.0)		, amf0_body);
	amf0_build_t::to_amf0(dvar_nil_t()		, amf0_body);
	// serialize the packet
	bytearray_t	bytearray;
	rtmp_build_t::serialize(rtmp_pkthd, amf0_body, bytearray);
	// return the datum_t describing the packet
	return bytearray.to_datum();
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
	case rtmp_type_t::PING:		return handle_ping(rtmp_event);
	default:	// simply ignore any other rtmp_type_t
			KLOG_ERR("rtmp_pkthd type is not handle =" << rtmp_pkthd);
			//DBG_ASSERT(false);
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
	// sanity check - at this point, rtmp_pkthd_t MUST BE rtmp_type_t::INVOKE
	DBG_ASSERT( rtmp_pkthd.type() == rtmp_type_t::INVOKE);

	// log to debug
	//KLOG_ERR("rtmp_pkthd="	<< rtmp_pkthd);	KLOG_ERR("pktbody="	<< pktbody);

	// parse the packet body
	bytearray_t	amf0_body(pktbody);
	dvar_t	dvar	= amf0_parse_t::parser(amf0_body);
	if( dvar.str().get() == "connect" ){
		return handle_invoke_connect(rtmp_pkthd, amf0_body);
	}else if( dvar.str().get() == "createStream" ){
		return handle_invoke_createStream(rtmp_pkthd, amf0_body);
	}else if( dvar.str().get() == "publish" ){
		return handle_invoke_publish(rtmp_pkthd, amf0_body);
	}else{
		KLOG_ERR("received INVOKE '" << dvar.str() << "' but not handled. remaining body=" << amf0_body);
		//DBG_ASSERT(false);
	}

	// return tokeep
	return true;
}

/** \brief handle rtmp_event_t for rtmp_type_t::INVOKE "connect"
 *
 * - highlevel call is invoke connect(double 1, object options)
 * - it is from NetConnection.connect("myconnect_uri") actionscript
 */
bool	rtmp_cam_full_t::handle_invoke_connect(const rtmp_pkthd_t &rtmp_pkthd
						, bytearray_t &amf0_body)	throw()
{
	rtmp_err_t	rtmp_err;
	// sanity check - at this point, rtmp_pkthd_t MUST BE rtmp_type_t::INVOKE
	DBG_ASSERT( rtmp_pkthd.type() == rtmp_type_t::INVOKE );

#if 1	// send a serverbw and clientbw
	// - apparently flash player got quota of bandwidth
	// - i failed to make them infinite so i set it to 0x7FFFFFFF
	// - any value greater than that is ignored by flash player
	// - quota of 250000 == able to send 2700000byte
	// - milgra set them to 250000

	// send a rtmp_type_t::CLIENTBW with 250000 - just in case
	datum_t		pkt_clientbw	= build_rtmp_clientbw(0x7FFFFFFF);
	rtmp_err	= rtmp_full->send(pkt_clientbw);
	DBG_ASSERT(rtmp_err.succeed());
	// send a rtmp_type_t::SERVERBW with 250000 - just in case
	// - not
	datum_t		pkt_serverbw	= build_rtmp_serverbw(0x7FFFFFFF);
	rtmp_err	= rtmp_full->send(pkt_serverbw);
	DBG_ASSERT(rtmp_err.succeed());
#endif

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
			.insert("application"	, dvar_nil_t())
			.insert("level"		, dvar_str_t("status"))
			.insert("description"	, dvar_str_t("Connection succeeded."))
			.insert("code"		, dvar_str_t("NetConnection.Connect.Success"))
							, amf0_resp);
	bytearray_t	data_resp;
	rtmp_build_t::serialize(rtmp_pkthd, amf0_resp, data_resp);
	// send the reply to the client
	rtmp_err	= rtmp_full->send(data_resp.to_datum());
	DBG_ASSERT(rtmp_err.succeed());

	// get the connect_uri from the connect parameter
	// - it is from NetConnection.connect(aUri) in actionscript
	// TODO this ASSERT is may cause useless crash ?
	DBG_ASSERT(param1.type().is_map());
	m_connect_uri	= param1.map()["tcUrl"].str().get();

	// log to debug
	KLOG_ERR("connect param1=" << param1);

#if 0	// not sure this is needed
	// send a invoke onBWDone - just in case
	rtmp_err	= rtmp_full->send( build_rtmp_onBWDone(rtmp_pkthd) );
	DBG_ASSERT(rtmp_err.succeed());
#endif

	// build and notify a rtmp_event_t::CONNECTED
	rtmp_event_t	rtmp_event;
	rtmp_event	= rtmp_event_t::build_connected(m_connect_uri);
	bool	tokeep	= notify_callback(rtmp_event);
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

/** \brief handle rtmp_event_t for rtmp_type_t::INVOKE "createStream"
 * - highlevel call is invoke createStream(double 2, object nil)
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

/** \brief handle rtmp_event_t for rtmp_type_t::INVOKE "publish"
 *
 * - highlevel call is publish(double 0, null, "resourcename", "options")
 * - it is from NetStream.publish("resourcename", "options") actionscript
 */
bool	rtmp_cam_full_t::handle_invoke_publish(const rtmp_pkthd_t &rtmp_pkthd
						, bytearray_t &amf0_body)	throw()
{
	// sanity check - at this point, rtmp_pkthd_t MUST BE rtmp_type_t::INVOKE
	DBG_ASSERT( rtmp_pkthd.type() == rtmp_type_t::INVOKE );

	// log to debug
	KLOG_ERR("rtmp_pkthd="	<< rtmp_pkthd);	KLOG_ERR("pktbody="	<< amf0_body);

	// get the parameter for the invoke("publish");
	dvar_t	param0	= amf0_parse_t::parser(amf0_body);
	dvar_t	param1	= amf0_parse_t::parser(amf0_body);
	dvar_t	rscname	= amf0_parse_t::parser(amf0_body);
	dvar_t	options	= amf0_parse_t::parser(amf0_body);
	// build the amf0_resp
	bytearray_t	amf0_resp;
	// build the INVOKE "publish" response
#if 0
	amf0_build_t::to_amf0(dvar_str_t("onStatus")	, amf0_resp);
	amf0_build_t::to_amf0(param0			, amf0_resp);
	amf0_build_t::to_amf0(dvar_nil_t()		, amf0_resp);
	amf0_build_t::to_amf0(dvar_map_t()
			.insert("level"		, dvar_str_t("status"))
			.insert("code"		, dvar_str_t("NetStream.Publish.Start"))
			.insert("details"	, dvar_str_t(rscname.str().get()))
			.insert("description"	, dvar_str_t(rscname.str().get() + " is now published."))
			.insert("clientid"	, dvar_dbl_t(rtmp_pkthd.stream_id()))
							, amf0_resp);
#else
	amf0_build_t::to_amf0(dvar_str_t("_result")	, amf0_resp);
	amf0_build_t::to_amf0(param0			, amf0_resp);
	amf0_build_t::to_amf0(dvar_nil_t()		, amf0_resp);
	amf0_build_t::to_amf0(dvar_dbl_t(1)		, amf0_resp);
#endif
	bytearray_t	data_resp;
	rtmp_build_t::serialize(rtmp_pkthd, amf0_resp, data_resp);
	// send the reply to the client
	rtmp_err_t	rtmp_err;
	rtmp_err	= rtmp_full->send(data_resp.to_datum());
	DBG_ASSERT(rtmp_err.succeed());

	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     handle rtmp_type_t::PING
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle rtmp_event_t for rtmp_type_t::PING
 */
bool	rtmp_cam_full_t::handle_ping(const rtmp_event_t &rtmp_event)	throw()
{
	// get the packet from the event
	datum_t		pktbody;
	rtmp_pkthd_t	rtmp_pkthd	= rtmp_event.get_packet(&pktbody);
	// sanity check - at this point, rtmp_pkthd_t MUST BE rtmp_type_t::PING
	DBG_ASSERT( rtmp_pkthd.type() == rtmp_type_t::PING);

// NOTE: this is a halfbacked attempt to answer ping
// - flash player stop streaming after 55sec for unknown reason


	// log to debug
	KLOG_ERR("rtmp_pkthd="	<< rtmp_pkthd);	KLOG_ERR("pktbody="	<< pktbody);
//	DBG_ASSERT(false);
//rtmp_pkthd=[channel_id=2 timestamp=4h3m51s339ms body_length=10 type=PING stream_id=0]
//pktbody=len=10 flag=[] data=
//0000: 00 03 00 00 00 01 00 00 00 00                     | ..........       |
// from http://www.google.com/codesearch?hl=en&q=onping+show:74yG6nMUPmg:Prr2nCGEhDk:74yG6nMUPmg&sa=N&cd=2&ct=rc&cs_p=http://fluorinefx.googlecode.com/svn&cs_f=trunk/Source/FluorineFx/Messaging/Rtmp/RtmpHandler.cs&exact_package=http://fluorinefx.googlecode.com/svn
// this means "client want to set the client buffer (00 03) of the stream_id 1 (00 00 00 01) to 0 (00 00 00 00)"

#if 1
{
	rtmp_pkthd_t	rtmp_pkthd;
	// build the header
	rtmp_pkthd.channel_id	( 0x02 );
	rtmp_pkthd.timestamp	( delay_t::from_msec(0));
	rtmp_pkthd.type		( rtmp_type_t::PING );
	rtmp_pkthd.stream_id	( 0 );
	// build the body (not in AMF0)
	bytearray_t	data_body;
	data_body <<	rtmp_ping_type_t(rtmp_ping_type_t::STREAMRESET);
	data_body <<	uint32_t(0x00000001);
	// serialize the packet
	bytearray_t	bytearray;
	rtmp_build_t::serialize(rtmp_pkthd, data_body, bytearray);
	// log to debug
	KLOG_ERR("rtmp_pkthd="	<< rtmp_pkthd);	KLOG_ERR("data_body="	<< data_body);
	// send the reply to the client
	rtmp_err_t	rtmp_err;
	rtmp_err	= rtmp_full->send(bytearray.to_datum());
	DBG_ASSERT(rtmp_err.succeed());
}
#endif
#if 1
{
	rtmp_pkthd_t	rtmp_pkthd;
	// build the header
	rtmp_pkthd.channel_id	( 0x02 );
	rtmp_pkthd.timestamp	( delay_t::from_msec(0));
	rtmp_pkthd.type		( rtmp_type_t::PING );
	rtmp_pkthd.stream_id	( 0 );
	// build the body (not in AMF0)
	bytearray_t	data_body;
	data_body <<	rtmp_ping_type_t(rtmp_ping_type_t::CLEAR);
	data_body <<	uint32_t(0x00000001);
	// serialize the packet
	bytearray_t	bytearray;
	rtmp_build_t::serialize(rtmp_pkthd, data_body, bytearray);
	// log to debug
	KLOG_ERR("rtmp_pkthd="	<< rtmp_pkthd);	KLOG_ERR("data_body="	<< data_body);
	// send the reply to the client
	rtmp_err_t	rtmp_err;
	rtmp_err	= rtmp_full->send(bytearray.to_datum());
	DBG_ASSERT(rtmp_err.succeed());
}
#endif
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






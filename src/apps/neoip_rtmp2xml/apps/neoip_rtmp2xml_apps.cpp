/*! \file
    \brief Definition of the \ref rtmp2xml_apps_t class

\par Brief Description
rtmp2xml_apps_t read .flv data from stdin and output the flv format into xml.
It is handcrafted xml but it is supposed to be valid one. aka parsable by
xml parser.
- it output all the info obtained by rtmp_event_t
- additionnaly it output a special element <keyframe> for each keyframe

\par TODO - the xml format is unclear.
  - the goal is to display all available info BUT to do it in a easyly parsable format
  - the current one fit current need and is generic... so not that bad for now

*/

/* system include */
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
/* local include */
#include "neoip_rtmp2xml_apps.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_rtmp.hpp"
#include "neoip_flv_amf0.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_dvar.hpp"
#include "neoip_dvar_helper.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_apps.hpp"
//#include "neoip_lib_apps_helper.hpp"

#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
rtmp2xml_apps_t::rtmp2xml_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	m_fdwatch	= NULL;
	m_rtmp_parse	= NULL;
}

/** \brief Destructor
 */
rtmp2xml_apps_t::~rtmp2xml_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the fdwatch if needed
	nipmem_zdelete m_fdwatch;
	// delete rtmp_event_t if needed
	nipmem_zdelete m_rtmp_parse;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
rtmp_err_t	rtmp2xml_apps_t::start()	throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	// log to debug
	KLOG_INFO("enter");

	// set all output options to false
	output_pkthd		= false;
	output_pkt_invoke	= false;
	output_pkt_ping		= false;
	// get the dest_dirpath from the command line option if specified
	if( arg_option.contain_key("pkthd") )		output_pkthd	= true;
	// if no option has been set, set them all
	if( !output_pkthd && !output_pkt_invoke && !output_pkt_ping ){
		output_pkthd		= true;
		output_pkt_invoke	= true;
		output_pkt_ping		= true;
	}

	// start the fdwatch on stdin
	m_fdwatch	= nipmem_new fdwatch_t();
	m_fdwatch->start(0, fdwatch_t::INPUT | fdwatch_t::ERROR, this, NULL);

	// open the rtmp_event_t
	rtmp_err_t	rtmp_err;
	m_rtmp_parse	= nipmem_new rtmp_parse_t();
	rtmp_err	= m_rtmp_parse->start(this, NULL);
	if( rtmp_err.failed() )	return rtmp_err;

	// output the xml result - here only a standard xml header
	KLOG_STDOUT("<?xml version=\"1.0\" encode=\"UTF-8\"?>\n");
	KLOG_STDOUT("<rtmp>\n");
	// return no error
	return rtmp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			misc
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/** \brief Exit the apps as soon as possible
 *
 * @return true in all case (only for tokeep convenience)
 */
bool	rtmp2xml_apps_t::exit_asap(const rtmp_err_t &rtmp_err = rtmp_err_t::OK)	throw()
{
	// if rtmp_err.failed(), log the event
	if( rtmp_err.failed() ){
		KLOG_STDOUT("ERROR " << rtmp_err.reason() << "\n");
		KLOG_ERR("Exit neoip-rtmp2xml due to " << rtmp_err);
	}else{
		// output the xml result
		KLOG_STDOUT("</rtmp>\n");
	}
	// stop the lib_session_t asap - to get the lib_session_exit_t from the rest of the code
	lib_session_get()->loop_stop_asap();

	// stop the post_fdwatch_zerotimer, if it exist
	post_fdwatch_zerotimer.remove(this, NULL);
	// delete the fdwatch if needed
	nipmem_zdelete m_fdwatch;
	// delete rtmp_event_t if needed
	nipmem_zdelete m_rtmp_parse;
	// return tokeep
	return false;
}

/** \brief Exit the apps as soon as possible - just helper on top of exit_asap(rtmp_err_t)
 */
bool	rtmp2xml_apps_t::exit_asap(const std::string &reason)	throw()
{
	return exit_asap(rtmp_err_t(rtmp_err_t::ERROR, reason));
}

/** \brief Return true if the parsing completed, false otherwise
 *
 * - the parsing is considered completed when:
 *   1. stdin is closed (aka m_fdwatch == NULL)
 *   2. rtmp_parse_t buffer is empty
 */
bool	rtmp2xml_apps_t::parsing_completed(const rtmp_event_t &rtmp_event)	throw()
{
#if 0	// compute the amount of unparsed data in rtmp_parse->buffer()
	size_t	unparsed_len	= m_rtmp_parse->buffer().size();

	// if a rtmp_event_t is specified, take its length into account
	if( !rtmp_event.is_null() ){
		DBG_ASSERT( m_rtmp_parse->buffer().size() >= rtmp_event.byte_length() );
		unparsed_len	-= rtmp_event.byte_length();
	}

	// log to debug
	KLOG_DBG("unparse_len=" << unparsed_len);

	// if unparsed_len == 0 and no more data to read, leave asap
	// - do a -4 because it should point to the rtmp_pkthd_t::type and
	//   not the rtmp_pkthd_t::prevtag_size which is an uint32_t
	//   - this rtmp_pkthd_t::prevtag_size crap is as well in
	//     rtmp_parse_helper_t::kframe_boffset_from(), in the rtmp_event_t nunit
	//     and neoip-rtmp2xml
	//   - i dont like it, seems like a bug in the rtmp_event_t architechture
	//   - it is spreaded everywhere, likely a misunderstanding of the flv format
	if( unparsed_len <= sizeof(uint32_t) && !m_fdwatch )	return true;
#endif
	// else return false
	return false;

}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			fdwatch_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief fdwatch callback
 */
bool	rtmp2xml_apps_t::neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cond)	throw()
{
	// log to debug
	KLOG_ERR("enter cond=" << cond);

	// sanity check - the fdwatch_t::INPUT condition MUST the one notified
	DBG_ASSERT( cond.is_input() );

	// allocate the buffer to store read data - in stack for better speed
	size_t	buf_len	= 128*1024;
	void *	buf_ptr	= nipmem_alloca(buf_len);
	// read the data on the fdwatch_t
	int	readlen	= read(m_fdwatch->get_fd(), buf_ptr, buf_len);

	// if there is an error, log the event and exit
	if( readlen < 0 && errno != EAGAIN ){
		std::string	reason = "read stdin error " + neoip_strerror(errno);
		return exit_asap(reason);
	}

	// test if std::cout has an error, if so, stop now
	// - NOTE: this typically happen if std::cout is a pipe and the other closed it
	if( std::cout.bad() )		return exit_asap("stdout has been closed");

	// if some data have been read, notify the caller
	if( readlen > 0 ){
		datum_t	read_data(buf_ptr, readlen, datum_t::NOCOPY);
		// feed the file_chunk to the rtmp_event_t
		m_rtmp_parse->notify_data(read_data);
	}

	// if it is the end of the stream, delete the fdwatch
	// - up to the rtmp_event_t callback to exit on last event
	if( readlen == 0 ){
		// delete fdwatch_t
		nipmem_zdelete	m_fdwatch;
		// if the parsing is completed, exit asap
		if( parsing_completed(rtmp_event_t()))	return exit_asap(rtmp_err_t::OK);
		// start the post_fdwatch_zerotimer
		post_fdwatch_zerotimer.append(this, NULL);
		// if not yet completed, let rtmp_event_t complete
		return	false;
	}

	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			zerotimer callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	rtmp2xml_apps_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw()
{
	// log to debug
	KLOG_ERR("enter");
	// sanity check - the m_fdwatch MUST be deleted at this point
	DBG_ASSERT( m_fdwatch == NULL );
	// sanity check - at this point, m_rtmp_parse MUST NOT be completed
	DBG_ASSERT( !parsing_completed(rtmp_event_t()) );

	// NOTE: this point is reached IIF m_fdwatch is closed and rtmp_event_t is still
	//       expecting data. so it is a bug in the input

	// exit with an error
	exit_asap("broken end of the input. input closed and rtmp_event_t still waiting for data");

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_event_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_parse_t when to notify an event
 */
bool	rtmp2xml_apps_t::neoip_rtmp_parse_cb(void *cb_userptr, rtmp_parse_t &cb_rtmp_parse
					, const rtmp_event_t &rtmp_event)	throw()
{
	rtmp_err_t	rtmp_err;
	// log to debug
	KLOG_DBG("enter event=" << rtmp_event);
	// sanity check - this event MUST be rtmp_event_t::is_parse_ok()
	DBG_ASSERT(rtmp_event.is_parse_ok());

	// if rtmp_event_t is fatal, report an error
	if( rtmp_event.is_fatal() )	return exit_asap(rtmp_event.get_error());

	// parse this event according to the rtmp_pkthd.type()
	switch( rtmp_event.value() ){
	case rtmp_event_t::PACKET:	rtmp_err = rtmp_event_packet(rtmp_event);	break;
	default:	DBG_ASSERT(0);
	}

	// handle the error case
	if( rtmp_err.failed() ){
		std::string reason = "unable to parse rtmp_event_t " + rtmp_event.to_string() + " due to " + rtmp_err.to_string();
		return	exit_asap(reason);
	}

	// if the parsing is completed, exit asap
	if( parsing_completed(rtmp_event) )	return exit_asap(rtmp_err_t::OK);

	// test if std::cout has an error, if so, stop now
	// - NOTE: this typically happen if std::cout is a pipe and the other closed it
	if( std::cout.bad() )		return exit_asap("stdout has been closed");

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_event_t handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the rtmp_event_t::PACKET
 */
rtmp_err_t	rtmp2xml_apps_t::rtmp_event_packet(const rtmp_event_t &rtmp_event)	throw()
{
	rtmp_err_t	rtmp_err;
	// sanity check - the rtmp_event MUST be a rtmp_event_t::PACKET
	DBG_ASSERT( rtmp_event.is_packet() );

	// get the packet itself
	datum_t		pkt_data;
	rtmp_pkthd_t	rtmp_pkthd	= rtmp_event.get_packet(&pkt_data);

	// if output_pkthd MUST OT be displayed, output the xml now
	if( output_pkthd ){
		KLOG_STDOUT("<pkthd>\n");
		KLOG_STDOUT("<channel_id>"	<< int(rtmp_pkthd.channel_id())			<< "</channel_id>\n");
		KLOG_STDOUT("<timestamp>"	<< rtmp_pkthd.timestamp().to_sec_double()	<< "</timestamp>\n");
		KLOG_STDOUT("<body_length>"	<< rtmp_pkthd.body_length()			<< "</body_length>\n");
		KLOG_STDOUT("<type>"		<< rtmp_pkthd.type()				<< "</type>\n");
		KLOG_STDOUT("<stream_id>"	<< rtmp_pkthd.stream_id()			<< "</stream_id>\n");
		KLOG_STDOUT("</pkthd>\n");
	}

	// TODO a lot here
	// - all the display
	// - all the cmdline option
	//   - doc them in the man page too

	// parse this event according to the rtmp_pkthd.type()
	switch( rtmp_pkthd.type().get_value() ){
	case rtmp_type_t::INVOKE:	rtmp_err	= parse_pkttype_invoke(rtmp_pkthd, pkt_data);
					break;
//	case rtmp_type_t::PING:		rtmp_err	= parse_pkttype_ping(rtmp_pkthd, pkt_data);
//					break;
	default:
		KLOG_STDOUT("unparsed pkttype " << rtmp_pkthd.type() << " pkthd=" << rtmp_pkthd
					<< " data=" << pkt_data << "\n");
		rtmp_err	= rtmp_err_t::OK;
	}
	// if an error occured, return now
	if( rtmp_err.failed() )	return rtmp_err;

	// return no error
	return rtmp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parse per rtmp_type_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse rtmp_type_t::INVOKE
 */
rtmp_err_t	rtmp2xml_apps_t::parse_pkttype_invoke(const rtmp_pkthd_t &rtmp_pkthd
						, const datum_t &pkt_data)	throw()
{
	// sanity check - rtmp_taghd_t MUST be a rtmp_type_t::INVOKE
	DBG_ASSERT( rtmp_pkthd.type() == rtmp_type_t::INVOKE );

	// parse the packet body
	bytearray_t	amf0_body(pkt_data);
	dvar_t	dvar	= amf0_parse_t::parser(amf0_body);
	if( dvar.str().get() == "connect" ){
		return parse_pkttype_invoke_connect(amf0_body);
	}else if( dvar.str().get() == "createStream" ){
		return parse_pkttype_invoke_createStream(amf0_body);
	}else if( dvar.str().get() == "publish" ){
		return parse_pkttype_invoke_publish(amf0_body);
	}else if( dvar.str().get() == "play" ){
		return parse_pkttype_invoke_play(amf0_body);
	}else{
		KLOG_STDOUT("received INVOKE '" << dvar.str() << "' but not handled. remaining body=" << amf0_body << "\n");
		// TODO to remove - to reenable
		//DBG_ASSERT(false);
	}

	// return no error
	return rtmp_err_t::OK;
}

/** \brief Parse rtmp_type_t::INVOKE and "connect"
 */
rtmp_err_t	rtmp2xml_apps_t::parse_pkttype_invoke_connect(bytearray_t &amf0_body)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// get the parameter for the invoke("connect");
	dvar_t	param0	= amf0_parse_t::parser(amf0_body);
	dvar_t	param1	= amf0_parse_t::parser(amf0_body);


	KLOG_STDOUT("connect(" << param0 << ", " << param1 << ")\n");

	// return no error
	return rtmp_err_t::OK;
}


/** \brief Parse rtmp_type_t::INVOKE and "createStream"
 */
rtmp_err_t	rtmp2xml_apps_t::parse_pkttype_invoke_createStream(bytearray_t &amf0_body)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// get the parameter for the invoke("connect");
	dvar_t	param0	= amf0_parse_t::parser(amf0_body);
	dvar_t	param1	= amf0_parse_t::parser(amf0_body);

	KLOG_STDOUT("createStream(" << param0 << ", " << param1 << ")\n");

	// return no error
	return rtmp_err_t::OK;
}

/** \brief Parse rtmp_type_t::INVOKE and "publish"
 */
rtmp_err_t	rtmp2xml_apps_t::parse_pkttype_invoke_publish(bytearray_t &amf0_body)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// get the parameter for the invoke("publish");
	dvar_t	param0	= amf0_parse_t::parser(amf0_body);
	dvar_t	param1	= amf0_parse_t::parser(amf0_body);
	dvar_t	rscname	= amf0_parse_t::parser(amf0_body);
	dvar_t	options	= amf0_parse_t::parser(amf0_body);

	KLOG_STDOUT("publish(" << param0 << ", " << param1 << ", " << rscname << ", " << options << ")\n");

	// return no error
	return rtmp_err_t::OK;
}

/** \brief Parse rtmp_type_t::INVOKE and "play"
 */
rtmp_err_t	rtmp2xml_apps_t::parse_pkttype_invoke_play(bytearray_t &amf0_body)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// get the parameter for the invoke("publish");
	dvar_t	param0	= amf0_parse_t::parser(amf0_body);
	dvar_t	param1	= amf0_parse_t::parser(amf0_body);
	dvar_t	param2	= amf0_parse_t::parser(amf0_body);

	KLOG_STDOUT("play(" << param0 << ", " << param1 << ", " << param2 << ")\n");

	// return no error
	return rtmp_err_t::OK;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	rtmp2xml_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --tophd cmdline option
	clineopt	= clineopt_t("pkthd", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("output the packet header");
	clineopt_arr	+= clineopt;

	// return the just built clineopt_arr_t
	return clineopt_arr;
}


NEOIP_NAMESPACE_END


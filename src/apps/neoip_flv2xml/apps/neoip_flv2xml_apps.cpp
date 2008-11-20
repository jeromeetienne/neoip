/*! \file
    \brief Definition of the \ref flv2xml_apps_t class

\par Brief Description
flv2xml_apps_t read .flv data from stdin and output the flv format into xml.
It is handcrafted xml but it is supposed to be valid one. aka parsable by
xml parser.
- it output all the info obtained by flv_parse_t
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
#include "neoip_flv2xml_apps.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_flv.hpp"
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
flv2xml_apps_t::flv2xml_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	m_fdwatch	= NULL;
	m_flv_parse	= NULL;
}

/** \brief Destructor
 */
flv2xml_apps_t::~flv2xml_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the fdwatch if needed
	nipmem_zdelete m_fdwatch;
	// delete flv_parse_t if needed
	nipmem_zdelete m_flv_parse;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
flv_err_t	flv2xml_apps_t::start()	throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	// log to debug
	KLOG_INFO("enter");

	// set all output options to false
	output_tophd	= output_tag_audio	= false;
	output_tag_video= output_tag_meta	= false;
	output_custom_kframe			= false;
	// get the dest_dirpath from the command line option if specified
	if( arg_option.contain_key("tophd") )		output_tophd		= true;
	if( arg_option.contain_key("tag_audio") )	output_tag_audio	= true;
	if( arg_option.contain_key("tag_video") )	output_tag_video	= true;
	if( arg_option.contain_key("tag_meta") )	output_tag_meta		= true;
	if( arg_option.contain_key("custom_kframe") )	output_custom_kframe	= true;
	// if no option has been set, set them all
	if( !output_tophd && !output_tag_audio && !output_tag_video
					&& !output_tag_meta && !output_custom_kframe){
		output_tophd	= output_tag_audio	= true;
		output_tag_video= output_tag_meta	= true;
		output_custom_kframe			= true;
	}

	// start the fdwatch on stdin
	m_fdwatch	= nipmem_new fdwatch_t();
	m_fdwatch->start(0, fdwatch_t::INPUT | fdwatch_t::ERROR, this, NULL);

	// open the flv_parse_t
	flv_err_t	flv_err;
	m_flv_parse	= nipmem_new flv_parse_t();
	flv_err		= m_flv_parse->start(this, NULL);
	if( flv_err.failed() )	return flv_err;

	// output the xml result - here only a standard xml header
	KLOG_STDOUT("<?xml version=\"1.0\" encode=\"UTF-8\"?>\n");
	KLOG_STDOUT("<flv>\n");
	// return no error
	return flv_err_t::OK;
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
bool	flv2xml_apps_t::exit_asap(const flv_err_t &flv_err = flv_err_t::OK)	throw()
{
	// if flv_err.failed(), log the event
	if( flv_err.failed() ){
		KLOG_STDOUT("ERROR " << flv_err.reason() << "\n");
		KLOG_ERR("Exit neoip-flv2xml due to " << flv_err);
	}else{
		// output the xml result
		KLOG_STDOUT("</flv>\n");
	}
	// stop the lib_session_t asap - to get the lib_session_exit_t from the rest of the code
	lib_session_get()->loop_stop_asap();

	// stop the post_fdwatch_zerotimer, if it exist
	post_fdwatch_zerotimer.remove(this, NULL);
	// delete the fdwatch if needed
	nipmem_zdelete m_fdwatch;
	// delete flv_parse_t if needed
	nipmem_zdelete m_flv_parse;
	// return tokeep
	return false;
}

/** \brief Exit the apps as soon as possible - just helper on top of exit_asap(flv_err_t)
 */
bool	flv2xml_apps_t::exit_asap(const std::string &reason)	throw()
{
	return exit_asap(flv_err_t(flv_err_t::ERROR, reason));
}

/** \brief Return true if the parsing completed, false otherwise
 *
 * - the parsing is considered completed when:
 *   1. stdin is closed (aka m_fdwatch == NULL)
 *   2. flv_parse_t buffer is empty
 */
bool	flv2xml_apps_t::parsing_completed(const flv_parse_event_t &parse_event)	throw()
{
	// compute the amount of unparsed data in flv_parse->buffer()
	size_t	unparsed_len	= m_flv_parse->buffer().size();

	// if a flv_parse_event_t is specified, take its length into account
	if( !parse_event.is_null() ){
		DBG_ASSERT( m_flv_parse->buffer().size() >= parse_event.byte_length() );
		unparsed_len	-= parse_event.byte_length();
	}

	// log to debug
	KLOG_DBG("unparse_len=" << unparsed_len);

	// if unparsed_len == 0 and no more data to read, leave asap
	// - do a -4 because it should point to the flv_taghd_t::type and
	//   not the flv_taghd_t::prevtag_size which is an uint32_t
	//   - this flv_taghd_t::prevtag_size crap is as well in
	//     flv_parse_helper_t::kframe_boffset_from(), in the flv_parse_t nunit
	//     and neoip-flv2xml
	//   - i dont like it, seems like a bug in the flv_parse_t architechture
	//   - it is spreaded everywhere, likely a misunderstanding of the flv format
	if( unparsed_len <= sizeof(uint32_t) && !m_fdwatch )	return true;
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
bool	flv2xml_apps_t::neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cond)	throw()
{
	// log to debug
	KLOG_DBG("enter cond=" << cond);

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
		datum_t	read_data(buf_ptr, readlen);
		// feed the file_chunk to the flv_parse_t
		m_flv_parse->notify_data(read_data);
	}

	// if it is the end of the stream, delete the fdwatch
	// - up to the flv_parse_t callback to exit on last event
	if( readlen == 0 ){
		// delete fdwatch_t
		nipmem_zdelete	m_fdwatch;
		// if the parsing is completed, exit asap
		if( parsing_completed(flv_parse_event_t()))	return exit_asap(flv_err_t::OK);
		// start the post_fdwatch_zerotimer
		post_fdwatch_zerotimer.append(this, NULL);
		// if not yet completed, let flv_parse_t complete
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
bool	flv2xml_apps_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw()
{
	// log to debug
	KLOG_ERR("enter");
	// sanity check - the m_fdwatch MUST be deleted at this point
	DBG_ASSERT( m_fdwatch == NULL );
	// sanity check - at this point, m_flv_parse MUST NOT be completed
	DBG_ASSERT( !parsing_completed(flv_parse_event_t()) );

	// NOTE: this point is reached IIF m_fdwatch is closed and flv_parse_t is still
	//       expecting data. so it is a bug in the input

	// exit with an error
	exit_asap("broken end of the input. input closed and flv_parse_t still waiting for data");

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			flv_parse_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref flv_parse_t when to notify an event
 */
bool	flv2xml_apps_t::neoip_flv_parse_cb(void *cb_userptr, flv_parse_t &cb_flv_parse
					, const flv_parse_event_t &parse_event)	throw()
{
	flv_err_t	flv_err;
	// log to debug
	KLOG_DBG("enter event=" << parse_event);

	// if flv_parse_event_t is fatal, report an error
	if( parse_event.is_fatal() )	return exit_asap(parse_event.get_error());

	// parse this event according to the flv_taghd.type()
	switch( parse_event.value() ){
	case flv_parse_event_t::TOPHD:	flv_err	= parse_event_tophd(parse_event);	break;
	case flv_parse_event_t::TAG:	flv_err	= parse_event_tag(parse_event);		break;
	default:	DBG_ASSERT(0);
	}

	// display kframe if available
	if( flv_err.succeed() )	flv_err	= display_kframe_ifavail(parse_event);

	// handle the error case
	if( flv_err.failed() ){
		std::string reason = "unabled to parse flv_parse_event_t " + parse_event.to_string() + " due to " + flv_err.to_string();
		return	exit_asap(reason);
	}

	// if the parsing is completed, exit asap
	if( parsing_completed(parse_event) )	return exit_asap(flv_err_t::OK);

	// test if std::cout has an error, if so, stop now
	// - NOTE: this typically happen if std::cout is a pipe and the other closed it
	if( std::cout.bad() )		return exit_asap("stdout has been closed");

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			flv_parse_event_t handlers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the flv_parse_event_t::TOPHD
 */
flv_err_t	flv2xml_apps_t::parse_event_tophd(const flv_parse_event_t &parse_event)throw()
{
	flv_tophd_t	flv_tophd	= parse_event.get_tophd();
	// sanity check - the parse_event MUST be a flv_parse_event_t::TOPHD
	DBG_ASSERT( parse_event.is_tophd() );

	// log to debug
	KLOG_INFO("flv_tophd=" << flv_tophd);

	// if flv_tophd_t MUST NOT be displayed, return now
	if( !output_tophd )	return flv_err_t::OK;
	// output the xml result
	KLOG_STDOUT("<tophd>\n");
	KLOG_STDOUT("<version>"		<< int(flv_tophd.version())		<< "</version>\n");
	KLOG_STDOUT("<has_video>"	<< (flv_tophd.flag().is_video()? 1:0)	<< "</has_video>\n");
	KLOG_STDOUT("<has_audio>"	<< (flv_tophd.flag().is_audio()? 1:0)	<< "</has_audio>\n");
	KLOG_STDOUT("</tophd>\n");

	// return no error
	return flv_err_t::OK;
}

/** \brief Parse the flv_parse_event_t::TAG
 */
flv_err_t	flv2xml_apps_t::parse_event_tag(const flv_parse_event_t &parse_event)	throw()
{
	datum_t		tag_data;
	flv_taghd_t	flv_taghd	= parse_event.get_tag(&tag_data);
	flv_err_t	flv_err;
	// sanity check - the parse_event MUST be a flv_parse_event_t::TAG
	DBG_ASSERT( parse_event.is_tag() );


	// parse this event according to the flv_taghd.type()
	switch( flv_taghd.type().get_value() ){
	case flv_tagtype_t::AUDIO:	flv_err	= parse_tagtype_audio(flv_taghd, tag_data);
					break;
	case flv_tagtype_t::VIDEO:	flv_err	= parse_tagtype_video(flv_taghd, tag_data);
					break;
	case flv_tagtype_t::META:	flv_err	= parse_tagtype_meta (flv_taghd, tag_data);
					break;
	default:	std::string reason	= "unknown flv_taghd.type() " + flv_taghd.type().to_string();
			flv_err	= flv_err_t(flv_err_t::ERROR, reason);
	}
	// if an error occured, return now
	if( flv_err.failed() )	return flv_err;

	// return no error
	return flv_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parser per flv_tagtype_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse flv_tagtype_t::AUDIO
 */
flv_err_t	flv2xml_apps_t::parse_tagtype_audio(const flv_taghd_t &flv_taghd
						, const datum_t &tag_data)	throw()
{
	flv_taghd_audio_t	taghd_audio;
	// sanity check - flv_taghd_t MUST be a flv_tagtype_t::AUDIO
	DBG_ASSERT( flv_taghd.type() == flv_tagtype_t::AUDIO );

	// parse the flv_taghd_audio_t from the tag_data
	try {
		bytearray_t	bytearray(tag_data);
		bytearray >> taghd_audio;
	}catch(const serial_except_t &e){
		std::string	reason	= "failed to parse flv_taghd_t " +flv_taghd.to_string()+" due to "+e.what();
		return flv_err_t(flv_err_t::ERROR, reason);
	}

	// log to debug
	KLOG_DBG("taghd_audio=" << taghd_audio);

	// if flv_tagtype_t::AUDIO MUST NOT be displayed, return now
	if( !output_tag_audio )	return flv_err_t::OK;
	// output the xml result
	display_taghd_beg(flv_taghd);
	KLOG_STDOUT("<taghd_audio>\n");
	KLOG_STDOUT("<soundtype>"	<< taghd_audio.soundtype()	<< "</soundtype>\n");
	KLOG_STDOUT("<soundsize>"	<< taghd_audio.soundsize()	<< "</soundsize>\n");
	KLOG_STDOUT("<soundrate>"	<< taghd_audio.soundrate()	<< "</soundrate>\n");
	KLOG_STDOUT("<soundformat>"	<< taghd_audio.soundformat()	<< "</soundformat>\n");
#if 0	// modification to output the data too
	KLOG_STDOUT("<data>\n");
	KLOG_STDOUT(tag_data << "\n");
	KLOG_STDOUT("</data>\n");
#endif
	KLOG_STDOUT("</taghd_audio>\n");
	display_taghd_end(flv_taghd);

	// return no error
	return flv_err_t::OK;
}

/** \brief Parse flv_tagtype_t::VIDEO
 */
flv_err_t	flv2xml_apps_t::parse_tagtype_video(const flv_taghd_t &flv_taghd
						, const datum_t &tag_data)	throw()
{
	flv_taghd_video_t	taghd_video;
	// sanity check - flv_taghd_t MUST be a flv_tagtype_t::VIDEO
	DBG_ASSERT( flv_taghd.type() == flv_tagtype_t::VIDEO );

	// parse the flv_taghd_video_t from the tag_data
	try {
		bytearray_t	bytearray(tag_data);
		bytearray >> taghd_video;
	}catch(const serial_except_t &e){
		std::string	reason	= "failed to parse flv_taghd_t " +flv_taghd.to_string()+" due to "+e.what();
		return flv_err_t(flv_err_t::ERROR, reason);
	}

	// log to debug
	KLOG_DBG("taghd_video=" << taghd_video);

	// if flv_tagtype_t::VIDEO MUST NOT be displayed, return now
	if( !output_tag_video )	return flv_err_t::OK;
	// output the xml result
	display_taghd_beg(flv_taghd);
	KLOG_STDOUT("<taghd_video>\n");
	KLOG_STDOUT("<codecid>"		<< taghd_video.codecid()	<< "</codecid>\n");
	KLOG_STDOUT("<frametype>"	<< taghd_video.frametype()	<< "</frametype>\n");
	KLOG_STDOUT("</taghd_video>\n");
	display_taghd_end(flv_taghd);
#if 0	// modification to output the data too
	KLOG_STDOUT("<data>\n");
	KLOG_STDOUT(tag_data << "\n");
	KLOG_STDOUT("</data>\n");
#endif
	// return no error
	return flv_err_t::OK;
}

/** \brief Parse flv_tagtype_t::META
 */
flv_err_t	flv2xml_apps_t::parse_tagtype_meta(const flv_taghd_t &flv_taghd
						, const datum_t &tag_data)	throw()
{
	bytearray_t	bytearray(tag_data);
	dvar_t		type_dvar;
	dvar_t		data_dvar;
	flv_err_t	flv_err;
	// sanity check - flv_taghd_t MUST be a flv_tagtype_t::META
	DBG_ASSERT( flv_taghd.type() == flv_tagtype_t::META);
	// get the event_type
	flv_err	= amf0_parse_t::amf_to_dvar(bytearray, type_dvar);
	if( flv_err.failed() )	return flv_err;

	// get the event_data associated with the event_type
	flv_err	= amf0_parse_t::amf_to_dvar(bytearray, data_dvar);
	if( flv_err.failed() )	return flv_err;

	// log to debug
	KLOG_STDOUT("METADATA event_type=" << type_dvar << " event_data=" << data_dvar);
	DBG_ASSERT(0);

	// if flv_tagtype_t::META MUST NOT be displayed, return now
	if( !output_tag_meta )	return flv_err_t::OK;
	// display the event
	display_taghd_beg(flv_taghd);
	KLOG_STDOUT("<metadata type=\"" << type_dvar.str().get() << "\">\n");
	KLOG_STDOUT(dvar_helper_t::to_xml(data_dvar));
	KLOG_STDOUT("</metadata>\n");
	display_taghd_end(flv_taghd);

	// return no error
	return flv_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Display kframe position if available
 */
flv_err_t flv2xml_apps_t::display_kframe_ifavail(const flv_parse_event_t &parse_event)
										throw()
{
	file_size_t	kframe_boffset;
	// test if it is a key frame appears
	kframe_boffset	= flv_parse_helper_t::kframe_boffset_from(m_flv_parse, parse_event);
	if( kframe_boffset.is_null() )	return flv_err_t::OK;

	// get the flv_taghd_t to know the timestamp of this kframe
	flv_taghd_t	flv_taghd	= parse_event.get_tag(NULL);

	// if custom_kframe MUST NOT be displayed, return now
	if( !output_custom_kframe )	return flv_err_t::OK;

	// display the event in STDOUT
	KLOG_STDOUT("<keyframe");
	KLOG_STDOUT(" timestamp=\"" << flv_taghd.timestamp().to_sec_double() << "\"");
	KLOG_STDOUT(" byte_offset=\"" << kframe_boffset << "\"");
	KLOG_STDOUT("></keyframe>\n");

	// return no error
	return flv_err_t::OK;
}

/** \brief Display the begining of a flv_taghd_t
 */
void	flv2xml_apps_t::display_taghd_beg(const flv_taghd_t &flv_taghd)		throw()
{
	KLOG_STDOUT("<taghd>\n");
	KLOG_STDOUT("<prevtag_size>"	<< flv_taghd.prevtag_size()		<< "</prevtag_size>\n");
	KLOG_STDOUT("<tagtype>"		<< flv_taghd.type()			<< "</tagtype>\n");
	KLOG_STDOUT("<body_length>"	<< flv_taghd.body_length()		<< "</body_length>\n");
	KLOG_STDOUT("<padding>"		<< flv_taghd.padding()			<< "</padding>\n");
	KLOG_STDOUT("<timestamp>"	<< flv_taghd.timestamp().to_sec_double()<< "</timestamp>\n");
}
/** \brief Display the end of a flv_taghd_t
 */
void	flv2xml_apps_t::display_taghd_end(const flv_taghd_t &flv_taghd)		throw()
{
	KLOG_STDOUT("</taghd>\n");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	flv2xml_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --tophd cmdline option
	clineopt	= clineopt_t("tophd", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("output the flv top header");
	clineopt_arr	+= clineopt;
	// add the --tag_audio cmdline option
	clineopt	= clineopt_t("tag_audio", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("output the flv tag audio");
	// add the --tag_video cmdline option
	clineopt	= clineopt_t("tag_video", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("output the flv tag video");
	clineopt_arr	+= clineopt;
	// add the --tag_meta cmdline option
	clineopt	= clineopt_t("tag_meta", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("output the flv tag meta");
	clineopt_arr	+= clineopt;
	// add the --custom_kframe cmdline option
	clineopt	= clineopt_t("custom_kframe", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("generate special xml tag for the keyframes");
	clineopt_arr	+= clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}


NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the \ref flv_parse_helper_t class
    
*/

/* system include */
/* local include */
#include "neoip_flv_parse_helper.hpp"
#include "neoip_flv_parse.hpp"
#include "neoip_flv_parse_event.hpp"
#include "neoip_flv_taghd_video.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Extract a key frame byte offset from a flv_parse_t and a flv_parse_event_t
 * 
 * @return a null file_size_t if this flv_parse_event_t is not a keyframe, else 
 *         return the byteoffset of this keyframe
 */
file_size_t	flv_parse_helper_t::kframe_boffset_from(flv_parse_t *flv_parse
					, const flv_parse_event_t &event)	throw()
{
	// if the event IS NOT a flv_parse_event_t::TAG, return a null file_size_t
	if( !event.is_tag() )	return file_size_t(); 

	// get the parameters from the event
	datum_t			tag_data;
	const flv_taghd_t &	taghd	= event.get_tag(&tag_data);

	// if this flv_taghd_t is not a flv_tagtype_t::VIDEO, leave
	if( taghd.type() != flv_tagtype_t::VIDEO )	return file_size_t();
			
	// parse the flv_taghd_video_t
	bytearray_t	tmp;
	tmp.work_on_data_nocopy(tag_data);
	flv_taghd_video_t	taghd_video;
	try{
		tmp >> taghd_video;
	}catch(const serial_except_t &e){
		KLOG_ERR("Unable to parse the flv_taghd_video_t.. should not happen");
		DBGNET_ASSERT( 0 );
		return file_size_t();
	}

	// if flv_taghd_video_t IS NOT a key frame, return a null file_size_t
	if( taghd_video.frametype() != flv_frametype_t::KEY )	return file_size_t();
	
	// start at the current parse_length (aka at the begining of this flv_taghd_t)
	file_size_t	kframe_boffset;
	kframe_boffset	 = flv_parse->parsed_length();
	// NOTE: do a +4 because it should point to the flv_taghd_t::type and 
	//   not the flv_taghd_t::prevtag_size which is an uint32_t
	//   - this flv_taghd_t::prevtag_size crap is as well in 
	//     flv_parse_helper_t::kframe_boffset_from(), in the flv_parse_t nunit
	//     and neoip-flvfilter 
	//   - i dont like it, seems like a bug in the flv_parse_t architechture
	//   - it is spreaded everywhere, likely a misunderstanding of the flv format
	kframe_boffset	+= sizeof(uint32_t);

	// log to debug
	KLOG_DBG("KEY frame at " << kframe_boffset);

	// return the kframe_boffset
	return kframe_boffset;
}

NEOIP_NAMESPACE_END



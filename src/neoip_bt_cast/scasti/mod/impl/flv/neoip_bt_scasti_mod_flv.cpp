/*! \file
    \brief Class to handle the bt_scasti_mod_flv_t

\par Brief Description
bt_scasti_mod_flv_t is a bt_scasti_mod_vapi_t which parse FLV flash movie files.
- It provides a prefix_header valid for FLV file with bt_scasti_mod_vapi_t::prefix_header()
- It extract the keyframe positions to export them with bt_scasti_mod_vapi_t::cast_spos_pop()
- the parsing is ensure by flv_parse_t

\par About the prefix_header
- BUILDING ALGO:
  - if no flv_tophd_t has been received from the stream, build an hardcoded one
  - if one flv_tophd_t has been received but no flv_tagtype_t::META, build a
    prefix_header which contains only the flv_tophd_t received
  - if one flv_tophd_t and the first flv_tagtype_t::META has been received
    build a prefix_header which contains both
- the prefix_header uses the flv_tophd_t from the input stream and the first
  flv_taghd_t which is a flv_tagtype_t::META
- the flv_tagtype_t::META body may provides interesting information about the stream
  - e.g. the resolution of the image which is used by the flash plugin player
  - or one can imagine stuff like the license of the stream or any other metainfo

*/

/* system include */
/* local include */
#include "neoip_bt_scasti_mod_flv.hpp"
#include "neoip_bt_scasti_vapi.hpp"
#include "neoip_bt_scasti_event.hpp"
#include "neoip_flv.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_scasti_mod_flv_t::bt_scasti_mod_flv_t()	throw()
{
	// zero some variable
	flv_parse	= NULL;
	m_type		= bt_scasti_mod_type_t::FLV;
}

/** \brief Destructor
 */
bt_scasti_mod_flv_t::~bt_scasti_mod_flv_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the flv_parse_t if needed
	nipmem_zdelete	flv_parse;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_scasti_mod_flv_t::start(bt_scasti_vapi_t *p_scasti_vapi)	throw()
{
	// copy the parameters
	this->m_scasti_vapi	= p_scasti_vapi;

	// initialize the flv_parse_t
	flv_err_t	flv_err;
	flv_parse	= nipmem_new flv_parse_t();
	flv_err		= flv_parse->start(this, NULL);
	if( flv_err.failed() )	return bt_err_from_flv(flv_err);
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_scasti_mod_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify new data to the this bt_scasti_mod_vapi_t
 */
void	bt_scasti_mod_flv_t::notify_data(const datum_t &data)	throw()
{
	// forward the data to the flv_parse_t
	flv_parse->notify_data(data);
}

/** \brief pop up a allowed start position - or null bt_cast_spos_t if none is available
 */
bt_cast_spos_t	bt_scasti_mod_flv_t::cast_spos_pop()		throw()
{
	// if no cast_spos_arr is queueed, return a null bt_cast_spos_t
	if( m_cast_spos_arr.empty() )	return bt_cast_spos_t();
	// unqueue the first cast_spos from the m_cast_spos_arr
	bt_cast_spos_t	cast_spos	= m_cast_spos_arr.pop_front();
	// return the result
	return cast_spos;
}

/** \brief return a datum_t containing prefix_header
 */
datum_t		bt_scasti_mod_flv_t::prefix_header()	const throw()
{
	// if no flv_tophd_t has been received, return a default prefix_header
	if( m_flv_tophd.is_null() ){
		// return a default header for FLV
		// - see http://www.osflash.org/flv for flv file format
		#define FLV_HEADER "FLV\x1\x1\0\0\0\x9\0\0\0\x9"	// magic header
		return	datum_t(FLV_HEADER, sizeof(FLV_HEADER)-1);
	}

	// if no flv_taghd_t of flv_tagtype_t::META, return a
	if( m_meta_taghd.is_null() ){
		bytearray_t	bytearray;
		// append the
		bytearray << m_flv_tophd;
		bytearray << uint32_t(0x09);
		// return the result
		return bytearray.to_datum();
	}


	// sanity check - at this point, all data for a 'full prefix_header' has been received
	DBG_ASSERT( !m_flv_tophd.is_null() );
	DBG_ASSERT( !m_meta_taghd.is_null() );
	DBG_ASSERT( !m_meta_tagdata.is_null() );


	bytearray_t	bytearray;
	// append the flv_tophd_t
	bytearray << m_flv_tophd;
	// append the flv_taghd_t for the flv_tagtype_t::META
	bytearray << m_meta_taghd;
	// append the body of the flv_tagtype_t::META
	bytearray.append(m_meta_tagdata);
	// append a 'prevtag_size' at the end of the prefix_header
	// - minus sizeof(uint32_t) because it doesnt count the 'prevtag_size'
	//   of the flv_taghd_t
	bytearray << uint32_t(m_meta_tagdata.size() + flv_taghd_t::TOTAL_LENGTH - sizeof(uint32_t));

	// log to debug
	KLOG_DBG("prefix_header=" << bytearray.to_datum());

	// return the result
	return bytearray.to_datum();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			flv_parse_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref flv_parse_t when to notify an event
 */
bool	bt_scasti_mod_flv_t::neoip_flv_parse_cb(void *cb_userptr, flv_parse_t &cb_flv_parse
					, const flv_parse_event_t &parse_event)	throw()
{
	bool	tokeep;
	// log to debug
	KLOG_DBG("enter event=" << parse_event);

	// if flv_parse_event_t is fatal, report an error
	if( parse_event.is_fatal() ){
		// sanity check - it MUST be a flv_parse_event_t::ERROR
		DBG_ASSERT( parse_event.is_error() );
		// get the error from the flv_parse_event_t
		bt_err_t	bt_err	= bt_err_from_flv(parse_event.get_error());
		bt_scasti_event_t event	= bt_scasti_event_t::build_error(bt_err);
		return m_scasti_vapi->mod_vapi_notify_callback(event);
	}

	// parse for the start position
	tokeep	= parse_for_cast_spos(parse_event);
	if( !tokeep )	return false;

	// parse for the prefix_header
	tokeep	= parse_for_prefix_header(parse_event);
	if( !tokeep )	return false;

	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			subparser
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief handler of flv_parse_event_t to build the m_cast_spos_arr
 */
bool	bt_scasti_mod_flv_t::parse_for_cast_spos(const flv_parse_event_t &parse_event)throw()
{
	// queue the kframe_boffset if this event contains a flv_taghd_video_t with keyframe
	file_size_t	kframe_boffset;
	kframe_boffset	= flv_parse_helper_t::kframe_boffset_from(flv_parse, parse_event);
	// if there are no kframe, return now
	if( kframe_boffset.is_null() )	return true;

	// log to debug
	KLOG_DBG("KEY frame at " << kframe_boffset);

	// append the keyframe byteoffset to the m_cast_spos_arr
	bt_cast_spos_t	cast_spos(kframe_boffset, date_t::present());
	m_cast_spos_arr.append(cast_spos);

	// notify the caller - that the bt_scasti_event_t::MOD_UPDATED
	bt_scasti_event_t scasti_event	= bt_scasti_event_t::build_mod_updated();
	bool	tokeep	= m_scasti_vapi->mod_vapi_notify_callback(scasti_event);
	if( !tokeep )	return false;

	// return a tokeep
	return true;
}

/** \brief handler of flv_parse_event_t to build the prefix_header
 */
bool	bt_scasti_mod_flv_t::parse_for_prefix_header(const flv_parse_event_t &parse_event)throw()
{
	// if the event IS a flv_parse_event_t::TOPHD, copy it locally and leave
	if( parse_event.is_tophd() && m_flv_tophd.is_null() ){
		m_flv_tophd	= parse_event.get_tophd();
		return true;
	}

	// sanity check - at this point, the flv_parse_event_t MUST BE a flv_parse_event_t::TAG
	DBG_ASSERT( parse_event.is_tag() );
	// get the parameters from the event
	datum_t			recved_tagdata;
	const flv_taghd_t &	recved_taghd	= parse_event.get_tag(&recved_tagdata);
	// if this flv_taghd_t is not a flv_tagtype_t::META, discard it
	if( recved_taghd.type() != flv_tagtype_t::META )	return true;

	// if a flv_tagtype_t::META has already been received, log the event and discard it
	if( !m_meta_taghd.is_null() ){
		KLOG_ERR("Received multiple flv_tagtype_t::META on this stream. discarding new ones");
		return true;
	}

	// NOTE: at this point, the just received event contains the first flv_tagtype_t::META

	// copy the data into the local variables
	m_meta_taghd	= recved_taghd;
	m_meta_tagdata	= recved_tagdata;
	// as m_meta_taghd will always be the first tag delivered, zero its prevtag_size
	m_meta_taghd.prevtag_size	( 0 );

	// log to debug
	KLOG_DBG("meta_taghd=" << m_meta_taghd);
	KLOG_DBG("meta_tagdata=" << m_meta_tagdata);

	// return a tokeep
	return true;
}

NEOIP_NAMESPACE_END






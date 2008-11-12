/*! \file
    \brief Class to handle the rtmp_parse_t

\par Brief Description
rtmp_parse_t is done to parse RTMP flash protocol.

\par reference of external documents
- documents giving example of rtmp packets
  - http://www.gnashdev.org/files/rtmp-decoded.pdf
    http://wiki.gnashdev.org/RTMP_Messages_Decoded
  - http://www.acmewebworks.com/Downloads/openCS/TheAMF.pdf
- a wireshark parser to study RTMP packet by sniffing networks
  - http://wiki.wireshark.org/RTMPT

\par example of implemenation
- source about rtmp encoding. nice to see rtmp packet format
  - http://code.google.com/p/fluorinefx/source/browse/trunk/Source/FluorineFx/Messaging/Rtmp/RtmpProtocolEncoder.cs?spec=svn9&r=9
- a server in
  - http://code.google.com/p/haxevideo/
- a python one currently halfbacked
  - http://rtmpy.org/wiki/Examples
- a java one. working well. able to read from webcam and save flv file
  - http://www.milgra.com

\par how to parse a packet
- assumption: rtmp_parse_t::m_buffer always start at the begining of a rtmp packet
- assumption: rtmp_parse_chanctx_t::m_buffer contains the body, possibly partial, of a rtmp_packet_t
- determine if rtmp_parse_t::m_buffer contains a rtmp_pkdhd_t
- from it, get the rtmp_parse_chanctx_t for the channel_id
- from the rtmp_parse_chanctx_t::m_last_pkthd and the just recieved rtmp_pkthd_t, get the real rtmp_pkthd_t
  - first init rtmp_pkthd_t with the last pkthd from the chanctx
  - then peek and copy data from the rtmp_parse_t and unserialize the rtmp_pkthd_t

- how to determine the size of the chunk present in the stream
  - remaining length = rtmp_pkdhd_t::body_size() - rtmp_parse_chanctx_t::body_buffer().length()
  - chunk length = min(remaining length, 128)
- if rtmp_parse_t::m_buffer contains the rtmp_pkthd_t and the whole chunk, go on
  - else return now
- pass both, chunk body + rtmp_pkthd_t to to chanctx
- chanctx update its last_pkthd_t with it
- the packet is complete IIF m_last_pkthd::body_length() == rtmp_parse_chanctx_t::body_buffer().length()
- if the packet is not complete, return now
- if the packet is complete, build an event and notify it

*/

/* system include */
/* local include */
#include "neoip_rtmp_parse.hpp"
#include "neoip_rtmp_parse_chanctx.hpp"
#include "neoip_rtmp_event.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
rtmp_parse_t::rtmp_parse_t()	throw()
{
	// init some variables
}

/** \brief Destructor
 */
rtmp_parse_t::~rtmp_parse_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// close all pending rtmp_parse_chanctx_t
	while( !chanctx_db.empty() )	nipmem_delete chanctx_db.front();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
rtmp_parse_t &	rtmp_parse_t::profile(const rtmp_parse_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );
	// copy the parameter
	this->m_profile	= profile;
	// return the object iself
	return *this;
}
/** \brief Start the operation
 */
rtmp_err_t	rtmp_parse_t::start(rtmp_parse_cb_t *callback, void *userptr) 	throw()
{
	// copy the parameters
	this->callback	= callback;
	this->userptr	= userptr;

	// return no error
	return rtmp_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   handle the chanctx_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the rtmp_parse_chanctx_t matching this channel_id, or NULL if none does
 */
rtmp_parse_chanctx_t	*rtmp_parse_t::chanctx_by_channel_id(const uint8_t &channel_id)	throw()
{
	std::list<rtmp_parse_chanctx_t *>::iterator	iter;
	// go thru the whole chanctx_db
	for(iter = chanctx_db.begin(); iter != chanctx_db.end(); iter++){
		rtmp_parse_chanctx_t *	parse_chanctx	= *iter;
		// if this parse_chanctx channel_id matches, return it
		if( channel_id == parse_chanctx->channel_id() )	return parse_chanctx;
	}
	// if this point is reached, no rtmp_parse_chanctx_t matches, so return NULL
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Notify new data into the rtmp_parse_t
 *
 * @return a rtmp_err_t. if it failed, it is syntax error. aka it should not happen
 *         in 'normal' operation.
 */
void	rtmp_parse_t::notify_data(const datum_t &new_data) 	throw()
{
	// log to debug
	KLOG_DBG("enter new_data.size()=" << new_data.size());

	// queue the new_data into the buffer
	m_buffer.append(new_data);

	// launch the zerotimer if not already running
	if( zerotimer.empty() )	zerotimer.append(this, NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////.

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	rtmp_parse_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw()
{
	// just forward to the parsing function
	return parse_buffer();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       parsing function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////.

/** \brief parse the buffer
 *
 * @return a tokeep for the rtmp_parse_t
 */
bool	rtmp_parse_t::parse_buffer() 	throw()
{
	// parse packet until it is no more possible
	while( true ){
		bool	stop_parsing	= true;
		// try to parse a packet
		bool	tokeep	= parse_packet(&stop_parsing);
		if( !tokeep )		return false;
		// if stop_parsing has been set to true, leave the loop
		if( stop_parsing )	break;
	};

	// if the buffer is > than the max, free it and notify an error
	// - this avoid a DoS in case of bogus input
	if( buffer().size() > profile().buffer_maxlen() ){
		m_buffer	= bytearray_t();
		rtmp_err_t	rtmp_err(rtmp_err_t::ERROR, "receiving buffer larger than " + OSTREAMSTR(profile().buffer_maxlen()));
		return	notify_callback( rtmp_event_t::build_error(rtmp_err) );
	}
	// return 'tokeep'
	return true;
}


/** \brief parse the buffer to get one packet
 *
 * - if tokeep is returned, *stop_parsing returns true if parsing must be stopped, false otherwise
 * - if dontkeep is returned, *stop_parsing is ignored
 *
 * @return a tokeep for the rtmp_parse_t
 */
bool	rtmp_parse_t::parse_packet(bool *stop_parsing) 	throw()
{
	static const size_t	pkthd_len_arr[]	= {12, 8, 4, 1};
	// if m_buffer doesnt even contain the channel_id, return now
	// - determine if rtmp_parse_t::m_buffer contains a rtmp_pkdhd_t
	if( m_buffer.length() < 1 )		return true;

// TODO to review if you dont get an error in the parsing
// - this is up to the caller to handle the timeout
// - the limitation of 12bytes for the rtmp_pkthd_t + 128byte for the chunk
//   length make an obvious error detection
// TODO there is still a profile with a BUFFER_MAXLEN ?!?!

	// get and parse the first byte
	uint8_t	first_byte	= *(m_buffer.char_ptr());
	size_t	pkthd_len_code	= (first_byte >> 6) & 0x3;
	uint8_t	channel_id	= first_byte & 0x3F;

	// get the length of the rtmp_pkdhd_t of this packet
	size_t	pkthd_len	= pkthd_len_arr[pkthd_len_code];

	// if m_buffer doesnt even contain the rtmp_pkthd_t, return now
	if( m_buffer.length() < pkthd_len )	return true;

	// get the rtmp_parse_chanctx_t for this channel_id
	rtmp_parse_chanctx_t *	parse_chanctx;
	parse_chanctx		= chanctx_by_channel_id(channel_id);
	// if there is no match, create a new one
	if( !parse_chanctx ){
		rtmp_err_t	rtmp_err;
		parse_chanctx	= nipmem_new rtmp_parse_chanctx_t();
		rtmp_err	= parse_chanctx->start(this, channel_id);
		DBG_ASSERT(rtmp_err.succeed());
	}

	// determine the rtmp_pkthd_t from m_buffer and parse_chanctx->last_pkthd()
	// - rtmp_pkthd_t unserial has been coded not to reset the output object and
	//   to handle the rtmp packet header compression.
	// - from the rtmp_parse_chanctx_t::m_last_pkthd and the just recieved rtmp_pkthd_t, get the real rtmp_pkthd_t
	//   - first init rtmp_pkthd_t with the last pkthd from the chanctx
	//   - then peek and copy data from the rtmp_parse_t and unserialize the rtmp_pkthd_t
	bytearray_t	pkthd_barray(m_buffer.range(0, pkthd_len));
	rtmp_pkthd_t	chunk_pkthd;
	chunk_pkthd	= parse_chanctx->last_pkthd();
	pkthd_barray	>> chunk_pkthd;
	KLOG_DBG("pkthd="<< pkthd_len << " data=" << m_buffer.range(0, pkthd_len));

	// determine the size of the chunk present in the stream
	// - remaining length = rtmp_pkdhd_t::body_size() - rtmp_parse_chanctx_t::body_buffer().length()
	// - chunk length = min(remaining length, 128)
	DBG_ASSERT( chunk_pkthd.body_length() >= parse_chanctx->body_buffer().length() );
	size_t	remaining_len	= chunk_pkthd.body_length() - parse_chanctx->body_buffer().length();
	size_t	chunk_len	= std::min(remaining_len, rtmp_pkthd_t::CHUNK_MAXLEN);

	// if m_buffer doesnt even contain the whole chunk, return now
	// - pkthd_len is still in the computation as it is still store in m_buffer
	if( m_buffer.length() < pkthd_len + chunk_len )	return true;

	// remove rtmp_pkthd_t from m_buffer
	m_buffer.head_free(pkthd_len);
	// consume the chunk_body
	datum_t	chunk_body	= m_buffer.head_consume(chunk_len);

	// set stop_parsing to false as a whole chunk has been removed from m_buffer
	*stop_parsing	= false;

	// notify chunk_pkthd_t+chunk_body to rtmp_parse_chanctx_t for this channel_id
	return parse_chanctx->notify_chunk(chunk_pkthd, chunk_body);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool rtmp_parse_t::notify_callback(const rtmp_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_rtmp_parse_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END






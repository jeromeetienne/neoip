/*! \file
    \brief Definition of the \ref rtmp_build_t class

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_rtmp_build.hpp"
#include "neoip_rtmp_pkthd.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief Helper on top of the other to_dvar
 *
 */
void	rtmp_build_t::serialize(const rtmp_pkthd_t &p_rtmp_pkthd, const datum_t &rtmp_body
						, bytearray_t &bytearray)	throw()
{
	rtmp_pkthd_t	rtmp_pkthd	= p_rtmp_pkthd;
	// set the body_length
	rtmp_pkthd.body_length(rtmp_body.length());

	// put the rtmp_pkthd_t in the stream
	// - the packet header7 compression is not handled
	bytearray	<< rtmp_pkthd;
	// copy the body with the chunk separator
	for(size_t offset = 0; offset < rtmp_body.length(); offset += rtmp_pkthd_t::CHUNK_MAXLEN){
		// if it is not the first chunk, add a separator
		if( offset > 0 )	bytearray << uint8_t(0xC0 | rtmp_pkthd.channel_id());
		// compute the chunk_len
		size_t	remaining	= rtmp_body.length() - offset;
		size_t	chunk_len	= std::min(remaining, rtmp_pkthd_t::CHUNK_MAXLEN);
		// append this chunk of data
		bytearray.append(rtmp_body.range(offset, chunk_len, datum_t::NOCOPY));
	}
	// log to debug
	KLOG_ERR("data=" << bytearray);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      helper
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Helper on top of the other to_dvar
 */
void	rtmp_build_t::serialize(const rtmp_pkthd_t &rtmp_pkthd, const bytearray_t &rtmp_body
						, bytearray_t &bytearray)	throw()
{
	serialize(rtmp_pkthd, rtmp_body.to_datum(datum_t::NOCOPY), bytearray);
}

/** \brief Helper on top of the other
 */
bytearray_t	rtmp_build_t::serialize(const rtmp_pkthd_t &rtmp_pkthd, const datum_t &rtmp_body)	throw()
{
	bytearray_t	bytearray;
	// serialize rtmp_pkthd+rtmp_body
	serialize(rtmp_pkthd, rtmp_body, bytearray);
	// return the result
	return bytearray;
}

NEOIP_NAMESPACE_END



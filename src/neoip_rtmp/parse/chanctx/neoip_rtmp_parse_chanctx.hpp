/*! \file
    \brief Header of the rtmp_parse_chanctx_t

*/


#ifndef __NEOIP_RTMP_PARSE_CHANCTX_HPP__
#define __NEOIP_RTMP_PARSE_CHANCTX_HPP__
/* system include */
/* local include */
#include "neoip_rtmp_pkthd.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_parse_t;

/** \brief class definition for rtmp_parse_chanctx_t
 */
class rtmp_parse_chanctx_t : NEOIP_COPY_CTOR_DENY{

private:
	rtmp_parse_t *	m_rtmp_parse;	//!< backpointer to the linked rtmp_parse_t
	uint8_t		m_channel_id;	//!< the channel_id for this rtmp_parse_chanctx_t
	rtmp_pkthd_t	m_last_pkthd;	//!< the last packet header (used for packet compression too)
	bytearray_t	m_body_buffer;	//!< the buffer of the packet body in progress
public:
	/*************** ctor/dtor	***************************************/
	rtmp_parse_chanctx_t() 		throw();
	~rtmp_parse_chanctx_t()		throw();

	/*************** setup function	***************************************/
	rtmp_err_t	start(rtmp_parse_t *rtmp_parse, uint8_t p_channel_id)	throw();

	/*************** query function	***************************************/
	const uint8_t &		channel_id()	const throw()	{ return m_channel_id;	}
	const rtmp_pkthd_t &	last_pkthd()	const throw()	{ return m_last_pkthd;	}
	const bytearray_t &	body_buffer()	const throw()	{ return m_body_buffer;	}

	/*************** Action function	*******************************/
	bool 			notify_chunk(const rtmp_pkthd_t &chunk_pkthd
						, const datum_t &chunk_data)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_PARSE_CHANCTX_HPP__  */




/*! \file
    \brief Class to handle the rtmp_parse_chanctx_t

\par Short Description
\ref rtmp_parse_chanctx_t parses the handshake for the connection received by rtmp_resp_t
*/

/* system include */
/* local include */
#include "neoip_rtmp_parse_chanctx.hpp"
#include "neoip_rtmp_parse.hpp"
#include "neoip_rtmp_event.hpp"
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
rtmp_parse_chanctx_t::rtmp_parse_chanctx_t()		throw()
{
	// zero some fields
	m_rtmp_parse	= NULL;
	m_channel_id	= 0;
}

/** \brief Destructor
 */
rtmp_parse_chanctx_t::~rtmp_parse_chanctx_t()		throw()
{
	// unlink this object from the rtmp_parse_t
	if( m_rtmp_parse )	m_rtmp_parse->chanctx_unlink(this);
}

/** \brief Autodelete the object and return false to ease readability
 */
bool	rtmp_parse_chanctx_t::autodelete()		throw()
{
	nipmem_delete	this;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
rtmp_err_t	rtmp_parse_chanctx_t::start(rtmp_parse_t *p_rtmp_parse, uint8_t p_channel_id)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// copy the parameter
	this->m_rtmp_parse	= p_rtmp_parse;
	this->m_channel_id	= p_channel_id;
	// sanity check - this channel_id MUST be unique per rtmp_parse_t
	DBG_ASSERT( !m_rtmp_parse->chanctx_by_channel_id(m_channel_id) );
	// dolink this object to the rtmp_parse_t
	m_rtmp_parse->chanctx_unlink(this);
	// return no error
	return rtmp_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         Actions function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief to notify a chunk of data by rtmp_parse_t
 *
 * @return a tokeep for rtmp_parse_t
 */
bool	rtmp_parse_chanctx_t::notify_chunk(const rtmp_pkthd_t &chunk_pkthd
					, const datum_t &chunk_body)	throw()
{
	// update m_last_pkthd with the new_pkthd
	m_last_pkthd	= chunk_pkthd;

	// append the chunk_body to m_body_buffer
	m_body_buffer.append(chunk_body);

	// TODO here make something to check the size of the chunk it not too much
	// - from a profile

	// m_body_buffer.length() MUST be <= m_last_pkthd.body_length() at all time
	if(m_body_buffer.length() > m_last_pkthd.body_length()){
		rtmp_err_t	rtmp_err(rtmp_err_t::ERROR, "actual rtmp pkt body length is greated than rtmp_pkthd_t one!");
		return m_rtmp_parse->notify_callback(rtmp_event_t::build_error(rtmp_err));
	}
	// sanity check - m_body_buffer.length() MUST be <= m_last_pkthd.body_length() at all time
	DBG_ASSERT(m_body_buffer.length() <= m_last_pkthd.body_length());

	// if the packet body is not complete return now
	// - pkt body is complete IIF m_last_pkthd::body_length() == rtmp_parse_chanctx_t::body_buffer().length()
	if( m_body_buffer.length() < m_last_pkthd.body_length() )	return true;

	// build and notify a rtmp_event_t::PACKET
	rtmp_event_t	rtmp_event;
	rtmp_event	= rtmp_event_t::build_packet(m_last_pkthd, m_body_buffer.to_datum(datum_t::NOCOPY));
	bool	tokeep	= m_rtmp_parse->notify_callback(rtmp_event);
	if( !tokeep )	return false;

	// remove the body from the m_body_buffer
	m_body_buffer.head_free( m_body_buffer.length() );
	DBG_ASSERT( m_body_buffer.length() == 0 );

	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END






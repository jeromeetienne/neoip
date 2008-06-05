/*! \file
    \brief Class to handle the http_sresp_cnx_t

*/

/* system include */
/* local include */
#include "neoip_http_sresp_cnx.hpp"
#include "neoip_http_sresp.hpp"
#include "neoip_http_sresp_ctx.hpp"
#include "neoip_http_resp.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
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
http_sresp_cnx_t::http_sresp_cnx_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	m_http_sresp	= NULL;
	m_socket_full	= NULL;
}

/** \brief Destructor
 */
http_sresp_cnx_t::~http_sresp_cnx_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the socket_full_t if needed
	nipmem_zdelete	m_socket_full;		
	// unlink this object from the http_sresp
	if( m_http_sresp )	m_http_sresp->cnx_unlink(this);
}

/** \brief Autodelete the object and return false to ease readability
 */
bool	http_sresp_cnx_t::autodelete(const std::string &reason)		throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR("autodelete due to " << reason);	
	// delete the object itself
	nipmem_delete	this;
	// return dontkeep for convenience
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
http_err_t	http_sresp_cnx_t::start(http_sresp_t *p_http_sresp, const http_reqhd_t &p_http_reqhd
					, socket_full_t *p_socket_full
					, const bytearray_t &p_recved_data)	throw()
{
	// log to debug
	KLOG_ERR("enter http_reqhd=" << p_http_reqhd);
	// copy the parameter
	this->m_http_sresp	= p_http_sresp;
	this->m_http_reqhd	= p_http_reqhd;
	this->m_socket_full	= p_socket_full;
	this->m_recved_data	= p_recved_data; 
	// link this object to the http_sresp
	m_http_sresp->cnx_dolink(this);
	
	// sanity check - recved_data MUST be empty IF http_method_t is NOT POST
	if( !m_recved_data.empty() && !m_http_reqhd.method().is_post() ){
		std::string	reason	= "recved data on a non POST connection";
		return http_err_t(http_err_t::ERROR, reason);
	}
	
	// set socket_full callback
	m_socket_full->set_callback(this, NULL);
	// setup the sendbuf maxlen for the socket_full_t
	m_socket_full->xmitbuf_maxlen( 50*1024 );
	
	// start the expire_timeout - usefull if the client is too long to get the reply
	expire_timeout.start(profile().expire_delay(), this, NULL);

	// if http_method_t::POST, http_reqhd MUST contain "Content-Length"
	if( m_http_reqhd.method().is_post() && !m_http_reqhd.has_header("Content-Length") ){
		std::string	reason	= "recved a POST with no Content-Length";
		return http_err_t(http_err_t::ERROR, reason);
	}

	// start the init_zerotimer
	init_zerotimer.append(this, NULL);
	
	// return no error
	return http_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the current profile for this object
 */
const http_sresp_cnx_profile_t & http_sresp_cnx_t::profile()	const throw()
{
	return m_http_sresp->profile().cnx();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////.

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	http_sresp_cnx_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw()
{
	// if m_http_reqhd.method().is_post(), parse the already received data and wait for more
	if( m_http_reqhd.method().is_post() )	return handle_recved_data(pkt_t());
	// else notify the http_srecp_ctx_t now
	return notify_ctx();	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       expire_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	http_sresp_cnx_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	return autodelete("expired after " + expire_timeout.get_period().to_string() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when a connection is established
 */
bool	http_sresp_cnx_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() )	return autodelete();
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:
			// data MUST NOT be called if it is not a POST
			if( !m_http_reqhd.method().is_post() )
				return autodelete("recved data on non POST");
			// forward to the proper handler
			return handle_recved_data(*socket_event.get_recved_data());
	case socket_event_t::MAYSEND_ON:
			// if MAYSEND_ON occurs and the maysend_threashold is == sendbuf maxlen, this is
			// the end of the connection, so autodelete
			// - this is a kludge to implement a weird linger... to fix
			if( m_socket_full->maysend_tshold() == m_socket_full->xmitbuf_maxlen())
				return autodelete();
			// try to full the socket_full sendbuf
			try_fill_xmitbuf();
			break;
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

/** \brief Handler RECVED_DATA from the socket_full_t
 */
bool	http_sresp_cnx_t::handle_recved_data(const pkt_t &pkt)	throw()
{
	// sanity check - data must be received IIF http_method_t::POST
	DBG_ASSERT( m_http_reqhd.method().is_post() );
	// queue the received data to the one already received
	m_recved_data.append( pkt.to_datum(datum_t::NOCOPY) );
	// some logging
	KLOG_ERR("recved data " << m_recved_data.length() << "-byte");
	// if the pending_data are too long, close the connection
	if( m_recved_data.length() > profile().recv_post_maxlen() )
		return autodelete("recved POST too large");
	// http_reqhd.content_length() MUST be size_t_ok
	if( !m_http_reqhd.content_length().is_size_t_ok() )
		return autodelete("recved Content-Length is not size_t ok");
	// get the http_reqhd.content_length()
	size_t	content_length	= m_http_reqhd.content_length().to_size_t();
#if 0
	// if the recved_data is larger than content_length, return an error
	if( m_recved_data.length() > content_length )
		return autodelete("recved_data larger than http_reqhd_t Content-Length");
#else
	// if recved_data is larger than content_length, strip m_recved_data to content_length
	// - NOTE: needed as IE (tested on IE6sp2/IE7) sends 2 additionnal byte "\r\n".
	if( m_recved_data.length() > content_length )
		m_recved_data.tail_free(m_recved_data.length() - content_length);
#endif
	// if the recved_data length is less than content_length, return now
	if( m_recved_data.length() < content_length )	return true;
	
	// now the http_method_t::POST is fully received, notify the http_sresp_ctx_t
	return notify_ctx();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Try to fill the send buf
 */
void	http_sresp_cnx_t::try_fill_xmitbuf()	throw()
{
	// determine the length to xmit
	size_t	len2xmit= m_socket_full->xmitbuf_freelen();
	len2xmit	= std::min(len2xmit, data2deliver.length());
	// if no data can be sent, return now
	if( len2xmit == 0 )	return;

	// write the data2post thru the socket_full_t
	socket_err_t	socket_err;
	socket_err	= m_socket_full->send(data2deliver.range(0, len2xmit, datum_t::NOCOPY));
	DBG_ASSERT( socket_err.succeed() );	// NOTE: this is only temporary this wont reutrn error in the future
	
	// free the data just delivered
	data2deliver.head_free(len2xmit);
	
	// if there are nomore data to deliver, init the maysend_tshold kludge to handle linger
	if( data2deliver.empty() )
		m_socket_full->maysend_tshold(m_socket_full->xmitbuf_maxlen());
}


/** \brief callback called when the \ref zerotimer_t expire
 */
bool	http_sresp_cnx_t::notify_ctx()		throw()
{
	http_sresp_ctx_t	sresp_ctx;
	/*************** build the sresp_ctx	*******************************/
	sresp_ctx.reqhd		( m_http_reqhd );
	sresp_ctx.post_data	( m_recved_data );
	// put a default http_rephd_t - up to the callback to change it if needed
	http_rephd_t	http_rephd;
	http_rephd.version(http_version_t::V1_0).status_code(200).reason_phrase("OK");
	sresp_ctx.rephd	( http_rephd );

	/*************** notify the caller	*******************************/
	bool	tokeep	= m_http_sresp->notify_callback(sresp_ctx);
	if( !tokeep )	return false;

	/*************** put caller response in socket	***********************/
	// if the content_length is not set by the callback, set it now
	if( !sresp_ctx.response_body().str().empty() && sresp_ctx.rephd().content_length().is_null() )
		sresp_ctx.rephd().content_length(sresp_ctx.response_body().str().size());

	// add the http_rephd_t and the filedatum in the data2deliver
	data2deliver.append(datum_t(sresp_ctx.rephd().to_http()));
	data2deliver.append(datum_t(sresp_ctx.response_body().str()));

	// set the maysend_threshold
	m_socket_full->maysend_tshold(m_socket_full->xmitbuf_maxlen()/2);
	// try to full the socket_full_t xmitbuf
	try_fill_xmitbuf();
	
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END






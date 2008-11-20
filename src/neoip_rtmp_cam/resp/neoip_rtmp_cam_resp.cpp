/*! \file
    \brief Definition of the \ref rtmp_cam_resp_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_cam_resp.hpp"
#include "neoip_rtmp_cam_listener.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
rtmp_cam_resp_t::rtmp_cam_resp_t()	throw()
{
	// zero some field
	m_cam_listener	= NULL;
}

/** \brief Destructor
 */
rtmp_cam_resp_t::~rtmp_cam_resp_t()	throw()
{
	// unlink this object from the rtmp_cam_listener_t
	if( m_cam_listener )	m_cam_listener->resp_unlink(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
rtmp_err_t	rtmp_cam_resp_t::start(rtmp_cam_listener_t *m_cam_listener
				, const http_uri_t &m_listen_uri
				, rtmp_cam_resp_cb_t *callback, void *userptr)	throw()
{
	// sanity check - m_listen_uri MUST be http_scheme_t::RTMP
	DBG_ASSERT( m_listen_uri.scheme().is_rtmp() );
	// log to debug
	KLOG_ERR("enter listen_uri=" << m_listen_uri);
	// copy the parameter
	this->m_cam_listener	= m_cam_listener;
	this->m_listen_uri	= m_listen_uri;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the rtmp_cam_listener_t
	m_cam_listener->resp_dolink(this);
	// return no error
	return rtmp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if this http_resp_t may handle this http_reqhd_t, false otherwise
 */
bool	rtmp_cam_resp_t::may_handle(const http_uri_t &p_req_uri)		const throw()
{
	// if m_listen_uri is != from req_uri without query part), then it is not possible to handle it
	if( m_listen_uri != http_uri_t(p_req_uri).clear_query()) 	return false;
	// return true if all the previous tests passed
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function called by the rtmp_cam_listener_t to notify a new connection for the rtmp_cam_resp_t
 *
 * - this function doesnt return any tokeep but as it notifies the user, the whole rtmp_cam_resp_t
 *   or rtmp_cam_listener_t may be deleted in the process.
 */
void	rtmp_cam_resp_t::notify_new_cnx(rtmp_cam_full_t *cam_full)	throw()
{
	// notify the event
	notify_callback(cam_full);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool rtmp_cam_resp_t::notify_callback(rtmp_cam_full_t *rtmp_cam_full)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_rtmp_cam_resp_cb(userptr, *this, rtmp_cam_full);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;





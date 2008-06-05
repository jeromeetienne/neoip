/*! \file
    \brief Definition of the \ref http_resp_t
    
*/

/* system include */
/* local include */
#include "neoip_http_resp.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
http_resp_t::http_resp_t()	throw()
{
	// zero some field
	m_http_listener	= NULL;
}

/** \brief Destructor
 */
http_resp_t::~http_resp_t()	throw()
{
	// unlink this object from the http_listener_t
	if( m_http_listener )	m_http_listener->resp_unlink(this);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
http_err_t	http_resp_t::start(http_listener_t *m_http_listener
				, const http_uri_t &m_listen_uri
				, const http_method_t &m_listen_method
				, const http_resp_mode_t &m_resp_mode
				, http_resp_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->m_http_listener	= m_http_listener;
	this->m_listen_uri	= m_listen_uri;
	this->m_listen_method	= m_listen_method;
	this->m_resp_mode	= m_resp_mode;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the http_listener_t
	m_http_listener->resp_dolink(this);
	// return no error
	return http_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if this http_resp_t may handle this http_reqhd_t, false otherwise
 */
bool	http_resp_t::may_handle(const http_reqhd_t &http_reqhd)		const throw()
{
	const http_uri_t &	req_uri	 = http_reqhd.uri();
	// log to debug
	KLOG_DBG("listen_uri="<< m_listen_uri);

	// if the method of the request header doesnt match the responder one, return false
	if( m_listen_method != http_method_t::NONE && http_reqhd.method() != m_listen_method )
		return false;
	// compare the uri http_scheme_t
	if( req_uri.scheme() != m_listen_uri.scheme() )		return false;
	// compare the uri host port if listen_uri.host() is not "0.0.0.0"
	if( m_listen_uri.host() != "0.0.0.0" ){
		// compare the uri host
		if( req_uri.host() != m_listen_uri.host() )	return false;
		// compare the uri port
		if( req_uri.port() != m_listen_uri.port() )	return false;
	}
	// compare the path - depends on the http_resp_mode_t	
	if( req_uri.path() != m_listen_uri.path() && m_resp_mode == http_resp_mode_t::REJECT_SUBPATH )
		return false;
	if( !m_listen_uri.path().contain(req_uri.path()) && m_resp_mode == http_resp_mode_t::ACCEPT_SUBPATH)
		return false;
	// return true if all the previous tests passed
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function called by the http_listener_t to notify a new connection for the http_resp_t
 * 
 * - this function doesnt return any tokeep but as it notifies the user, the whole http_resp_t
 *   or http_listener_t may be deleted in the process.
 */
void	http_resp_t::notify_new_cnx(const http_reqhd_t &http_reqhd, socket_full_t *socket_full
					, const bytearray_t &recved_data)	throw()
{
	// notify the event
	notify_callback(http_reqhd, socket_full, recved_data);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool http_resp_t::notify_callback(const http_reqhd_t &http_reqhd, socket_full_t *socket_full
					, const bytearray_t &recved_data)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_http_resp_cb(userptr, *this, http_reqhd, socket_full, recved_data);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;





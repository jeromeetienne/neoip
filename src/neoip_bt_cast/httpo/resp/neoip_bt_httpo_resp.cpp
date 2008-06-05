/*! \file
    \brief Class to handle the bt_httpo_resp_t

\par About supported http_method_t : GET and HEAD
- http_method_t::GET and http_method_t::HEAD are transparently supported by bt_httpo_*_t 
- both http_method_t::GET and http_method_t::HEAD are supported for every 
  http_uri_t a bt_httpo_resp_t is listening on.
- additionnaly bt_httpo_full_t handles the http_method_t::HEAD 
  - simply by not sending any http body after the head and by notifying a 

*/

/* system include */
/* local include */
#include "neoip_bt_httpo_resp.hpp"
#include "neoip_bt_httpo_event.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_listener.hpp"
#include "neoip_http_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_bytearray.hpp"
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
bt_httpo_resp_t::bt_httpo_resp_t()	throw()
{
	// zero some field
	m_httpo_listener	= NULL;
	m_resp_get		= NULL;
	m_resp_head		= NULL;
}

/** \brief Destructor
 */
bt_httpo_resp_t::~bt_httpo_resp_t()		throw()
{
	// unlink it from the bt_httpo_listener_t if needed
	if( m_httpo_listener )	m_httpo_listener->resp_unlink(this);
	// delete the m_resp_get if needed
	nipmem_zdelete	m_resp_get;
	// delete the m_resp_head if needed
	nipmem_zdelete	m_resp_head;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  Setup start
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_httpo_resp_t::start(bt_httpo_listener_t *httpo_listener, const http_uri_t &http_uri
					, const http_resp_mode_t &http_resp_mode
					, bt_httpo_resp_cb_t *callback, void *userptr)	throw()
{
	http_err_t	http_err;
	// copy the parameter
	this->m_httpo_listener	= httpo_listener;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the bt_httpo_listener_t
	m_httpo_listener->resp_dolink(this);
	// create the http_method_t::GET http_resp_t for this http_uri and http_resp_mode_t
	m_resp_get	= nipmem_new http_resp_t();
	http_err	= m_resp_get->start(m_httpo_listener->http_listener, http_uri
					, http_method_t::GET, http_resp_mode, this, NULL);
	if( http_err.failed() )	return bt_err_from_http(http_err);
	// create the http_method_t::HEAD http_resp_t for this http_uri and http_resp_mode_t
	m_resp_head	= nipmem_new http_resp_t();
	http_err	= m_resp_head->start(m_httpo_listener->http_listener, http_uri
					, http_method_t::HEAD, http_resp_mode, this, NULL);
	if( http_err.failed() )	return bt_err_from_http(http_err);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         http_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_resp_t when to notify an event
 * 
 * @return true if the http_resp_t is still valid after the callback
 */
bool	bt_httpo_resp_t::neoip_http_resp_cb(void *cb_userptr, http_resp_t &cb_http_resp
					, const http_reqhd_t &http_reqhd, socket_full_t *socket_full
					, const bytearray_t &recved_data)	throw()
{
	// log to debug
	KLOG_DBG("enter http_reqhd=" << http_reqhd);
	
	// sanity check - the recved_data MUST be empty 
	// - as it is only http_method_t::GET or http_method_t::HEAD
	if( !recved_data.empty() ){
		// assert only on debug as it depends on external data
		DBGNET_ASSERT( recved_data.empty() );
		// delete the socket
		nipmem_delete socket_full;
		return true;
	}

	// create a bt_httpo_full_t and notify it 
	bt_httpo_full_t *httpo_full;
	httpo_full	= nipmem_new bt_httpo_full_t(this, socket_full, http_reqhd);
	bool	tokeep	= notify_callback( bt_httpo_event_t::build_cnx_established(httpo_full) );
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_httpo_resp_t::notify_callback(const bt_httpo_event_t &httpo_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_httpo_resp_cb(userptr, *this, httpo_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END






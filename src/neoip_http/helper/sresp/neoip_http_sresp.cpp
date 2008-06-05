/*! \file
    \brief Definition of the \ref http_sresp_t

\par Brief Description
http_sresp_t is a simple insync http_resp_t. It allows to handle simple code
when using insync http request. There is a nunit for it in 
the http_sclient_testclass_t.

\par TODO to port the old httpd_t on top of this
- the http_sresp_ctx_t is is likely to be completed when porting the stuff
- once well done, replace the old httpd_t with this one
  - see the current usage of httpd_t
  - mainly in the wikidbg stuff
  - usage for httpd_err_t special error ?!?!
    - do i still use those ?
  - usage of mime type ?
  - to port it in wikidbg, port it gradually
    - httpd_err_t in wikidbg means 'still use the old httpd_t'
    - make another api for the wikidbg page to use the new http_sresp_t
    - static httpd_err_t page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
    - static http_err_t page(const std::string &keyword, void *object_ptr, http_sresp_req_t &request)	throw();
  - another usage is the xmlrpc stuff
  
*/

/* system include */
/* local include */
#include "neoip_http_sresp.hpp"
#include "neoip_http_sresp_cnx.hpp"
#include "neoip_http_resp.hpp"
#include "neoip_http_err.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
http_sresp_t::http_sresp_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some variables
	m_http_resp	= NULL;
}

/** \brief Destructor
 */
http_sresp_t::~http_sresp_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// close all pending http_sresp_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
	// delete http_resp_t if needed
	nipmem_zdelete	m_http_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
http_sresp_t &	http_sresp_t::profile(const http_sresp_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == http_err_t::OK );	
	// copy the parameter
	this->m_profile	= profile;
	// return the object iself
	return *this;
}
/** \brief Start the operation
 */
http_err_t http_sresp_t::start(http_listener_t *http_listener, const http_uri_t &listen_uri
			, const http_method_t &listen_method, const http_resp_mode_t &resp_mode
			, http_sresp_cb_t *callback, void *userptr)		throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// copy the parameters
	this->callback	= callback;
	this->userptr	= userptr;
	
	// create the http_resp_t for this http_uri and http_resp_mode_t
	http_err_t	http_err;
	m_http_resp	= nipmem_new http_resp_t();
	http_err	= m_http_resp->start(http_listener, listen_uri, listen_method
						, resp_mode, this, NULL);
	if( http_err.failed() )	return http_err;
	
	// return no error
	return http_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the listen http_uri_t
 */
const http_uri_t &	http_sresp_t::listen_uri()	const throw()
{
	return m_http_resp->listen_uri();
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
bool	http_sresp_t::neoip_http_resp_cb(void *cb_userptr, http_resp_t &cb_http_resp
				, const http_reqhd_t &http_reqhd, socket_full_t *socket_full
				, const bytearray_t &recved_data)		throw()
{
	// log to debug
	KLOG_DBG("enter http_reqhd=" << http_reqhd);

	// create a http_sresp_cnx_t and notify it 
	http_sresp_cnx_t *	sresp_cnx;
	http_err_t		http_err;
	sresp_cnx	= nipmem_new http_sresp_cnx_t();
	http_err	= sresp_cnx->start(this, http_reqhd, socket_full, recved_data);
	if( http_err.failed() ){
		KLOG_ERR("Cant start sresp_cnx due to " << http_err);
		nipmem_delete sresp_cnx;
	}

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 * 
 * - NOTE: this is called directly from http_sresp_cnx_t
 */
bool http_sresp_t::notify_callback(http_sresp_ctx_t &sresp_ctx)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_http_sresp_cb(userptr, *this, sresp_ctx);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END;





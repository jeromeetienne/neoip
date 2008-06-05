/*! \file
    \brief Class to handle the http_sclient_mod_raw_t

- TODO to comment this

*/

/* system include */
/* local include */
#include "neoip_http_sclient_mod_raw.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_http_sclient_res.hpp"
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
http_sclient_mod_raw_t::http_sclient_mod_raw_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some variable
	m_http_sclient	= NULL;
	m_type		= http_sclient_mod_type_t::RAW;
}

/** \brief Destructor
 */
http_sclient_mod_raw_t::~http_sclient_mod_raw_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
http_err_t	http_sclient_mod_raw_t::start(http_sclient_t *m_http_sclient)	throw()
{
	http_err_t	http_err;
	// copy the parameters
	this->m_http_sclient	= m_http_sclient;

	// return no error
	return http_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			http_sclient_mod_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief hook called to convert http_reqhd_t from a caller view to a server view
 * 
 * @return an http_err_t if http_err.failed() stop processing immediatly
 */

http_err_t http_sclient_mod_raw_t::reqhd_caller2server(http_reqhd_t *http_reqhd_ptr) const throw()
{
	// return no error
	return http_err_t::OK;	
}

/** \brief hook called to convert http_rephd_t from a server view to a caller view
 * 
 * @return an http_err_t if http_err.failed() stop processing immediatly
 */
http_err_t http_sclient_mod_raw_t::rephd_server2caller(http_rephd_t *http_rephd_ptr
					, const http_reqhd_t &caller_reqhd)	const throw()
{
	// return no error
	return http_err_t::OK;		
}

/** \brief hook called to get the 'internal' content_length from a server http_rephd_t
 * 
 * @return a file_size_t if file_size.is_null() then stop processing immediatly
 */
file_size_t http_sclient_mod_raw_t::contentlen_server2internal(const http_rephd_t &server_rephd
					, const http_reqhd_t &caller_reqhd)	throw()
{
	// return the server_contentlen
	return server_rephd.content_length();
}

/** \brief hook called to convert reply_body from a server view to a caller view
 * 
 * @return an http_err_t if http_err.failed() stop processing immediatly
 */
http_err_t	http_sclient_mod_raw_t::reply_body_server2caller(bytearray_t &reply_body
					, const http_reqhd_t &caller_reqhd)	throw()
{
	// return no error
	return http_err_t::OK;	
}
NEOIP_NAMESPACE_END






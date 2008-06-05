/*! \file
    \brief Class to handle the http_sclient_mod_flv_t

\par Brief Description
http_sclient_mod_flv_t aims to allows to *transparently* simulate standard 
http range-request on server which only support flv range-request.
- flv range-request is a mechanism developped to workaround a bug/feature of the
  flash plugin, namely it is not able to perform standard http range-request
  - the deployed flv range-request is appending a specific variable in the
    http uri in order to specify the begining of the range-request
  - it is not possible to specify the end of the range-request
  - depending on the server, the key of the variable is different. The currently
    known keys are "pos", "start", "position"
- the fact that the flv range-request is performed transparently by http_sclient_t
  allow to perform standard http range-request without caring about underlying 
  mechanism. 
  - NOTE: rfc2616.14.35 allows to have multiple ranges in a single request. 
    - they are not supported by http_sclient_mod_flv_t or http_sclient_t
- about naming, as this object convert transparently a good naming of the layer
  is needed to avoid confusion.
  - the data which are from/to the caller are tagged 'caller'
  - the data which are from/to the server are tagger 'server'

\par Assumption about the server
The server for this http_sclient_mod_flv_t is assumed not to be able to answer 
to normal range requests. 
- if it is, mod_flv is useless, use http_sclient_mod_type_t::RAW

\par About "http_mod_flv_varname" in http_sclient_t::m_metavar_db
- this allows the http_uri_t to specify the variable name to use to specify the 
  begining of the range
- some use "pos", "start" or "position"
- this is tunable to ensure flexibility/adaptability
- it defaults to "pos" as it seems to be the most popular
  - usual stream.php and lighttpd use it



\par TODO
- concept and arch seem rather good. but coded with no brain... so unsure
  it works well. have a nunit to test that
- i got 'minimal flv header' in several place
  - would be good to centralize it
  - in neoip_oload_mod_flv_t
  - in http_sclient_mod_flv_t
  - maybe in libneoip_flv.so too
- something very similar is implemented in neoip_oload_mod_flv_t
  - neoip_oload_mod_flv_t is the server part in neoip-oload
  - http_sclient_mod_flv_t is the client part

  
*/

/* system include */
/* local include */
#include "neoip_http_sclient_mod_flv.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_http_sclient_res.hpp"
#include "neoip_file_range.hpp"
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
http_sclient_mod_flv_t::http_sclient_mod_flv_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some variable
	m_http_sclient	= NULL;
	m_type		= http_sclient_mod_type_t::FLV;
	m_flvhd_removed	= false;
}

/** \brief Destructor
 */
http_sclient_mod_flv_t::~http_sclient_mod_flv_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
http_err_t	http_sclient_mod_flv_t::start(http_sclient_t *m_http_sclient)	throw()
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
http_err_t http_sclient_mod_flv_t::reqhd_caller2server(http_reqhd_t *http_reqhd_ptr) const throw()
{
	http_reqhd_t &	http_reqhd	= *http_reqhd_ptr;
	// log to debug
	KLOG_DBG("enter http_reqhd=" << http_reqhd);
	
	// if http_method_t::POST, return an error
	if( http_reqhd.method().is_post() )	return http_err_t(http_err_t::ERROR, "POST method is not allowed for http_sclient_mod_flv_t");
	// sanity check - the http_method_t MUST be GET or HEAD
	DBG_ASSERT( http_reqhd.method().is_get() || http_reqhd.method().is_head() );

	// get the requested range from the http_reqhd_t
	file_range_t	req_range	= http_reqhd.range();
	// if there is no requested range, do nothing and return now
	if( req_range.is_null() )	return http_err_t::OK;
		
	// if requested range starts after 0, append a http_uri_t variable with req_range.beg()
	if( req_range.beg() > 0 ){
		const strvar_db_t & metavar_db	= m_http_sclient->metavar_db();
		const std::string & var_name	= metavar_db.get_first_value("http_mod_flv_varname", "pos");
		http_reqhd.uri().var().append(var_name, req_range.beg().to_string());
	}
	
	// zero the req_range in http_reqhd_t - should be done in ALL cases
	// - some server for flv do not support http range requests. e.g. youtube
	http_reqhd.range( file_range_t() );
	
	// return no error
	return http_err_t::OK;	
}

/** \brief hook called to convert http_rephd_t from a server view to a caller view
 * 
 * @return an http_err_t if http_err.failed() stop processing immediatly
 */
http_err_t http_sclient_mod_flv_t::rephd_server2caller(http_rephd_t *http_rephd_ptr
					, const http_reqhd_t &caller_reqhd)	const throw()
{
	http_rephd_t &	http_rephd	= *http_rephd_ptr;
	// http_rephd_t MUST NOT be chunked_encoded
	if( http_rephd.is_chunked_encoded() )	return http_err_t(http_err_t::ERROR, "chunked_encoded is not allowed for http_sclient_mod_flv_t");
	// mark the http_rephd_t as accepting range
	http_rephd.accept_ranges(true);
	// if caller_reqhd has no range, do nothing and return now
	if( caller_reqhd.range().is_null() )	return http_err_t::OK;
	// if http_rephd.status_code == 200, change it to 206
	if( http_rephd.status_code() == 200 )	http_rephd.status_code(206);
	// set the content-range
	// - NOTE: the instance_len is unknown as not provided by the server
	http_rephd.content_range(caller_reqhd.range(), file_size_t());
	// set the content_length
	http_rephd.content_length(caller_reqhd.range().length());
	// return no error
	return http_err_t::OK;		
}

/** \brief hook called to get the 'internal' content_length from a server http_rephd_t
 * 
 * @return a file_size_t if file_size.is_null() then stop processing immediatly
 */
file_size_t http_sclient_mod_flv_t::contentlen_server2internal(const http_rephd_t &server_rephd
					, const http_reqhd_t &caller_reqhd)	throw()
{
	// get the range requested by the caller
	m_caller_range	= caller_reqhd.range();

	// sanity check - the m_caller_range MUST be fully defined
	DBG_ASSERT( m_caller_range.beg().is_null() == false	);
	DBG_ASSERT( m_caller_range.beg() != file_size_t::MAX 	);
	DBG_ASSERT( m_caller_range.end() != file_size_t::MAX 	);

	// return the content length as the length of the range requested by the caller
	return	m_caller_range.length();
}

/** \brief hook called to convert reply_body from a server view to a caller view
 * 
 * @return an http_err_t if http_err.failed() stop processing immediatly
 */
http_err_t	http_sclient_mod_flv_t::reply_body_server2caller(bytearray_t &reply_body
					, const http_reqhd_t &caller_reqhd)	throw()
{
	// define the FLV_MINIMAL_HEADER
	// - see http://www.osflash.org/flv for flv file format
	// - TODO this should be somewhere else
	#define FLV_MINIMAL_HEADER 	"FLV\x1\x1\0\0\0\x9\0\0\0\x9"	// magic header
	#define FLV_MINIMAL_HEADER_LEN	(sizeof(FLV_MINIMAL_HEADER)-1)


	// if the caller_range.beg() > 0, then remove FLV_MINIMAL_HEADER if not yet done 
	if( m_caller_range.beg() > 0 && !m_flvhd_removed && reply_body.size() > FLV_MINIMAL_HEADER_LEN){
		// mark that it has been removed
		m_flvhd_removed	= true;
		// remove the FLV_MINIMAL_HEADER_LEN from the reply_body head
		reply_body.head_free(FLV_MINIMAL_HEADER_LEN);
	}

	// if reply_body is larger than caller_range, remove the exceeding part
	// - NOTE: this may happen as the server send all until the end of the content
	//   and not all until the end of the range
	DBG_ASSERT( m_caller_range.length().is_size_t_ok() );
	if( reply_body.length() > m_caller_range.length().to_size_t() ){
		size_t	exceed_len = reply_body.length() - m_caller_range.length().to_size_t();
		reply_body.tail_free(exceed_len);
	}

	// return no error
	return http_err_t::OK;	
}


NEOIP_NAMESPACE_END






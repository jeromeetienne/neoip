/*! \file
    \brief Definition of the class \ref x509_request_t

\par Brief description
This modules handle the certificate request. it is used to generate a x509_cert_t
from a x509_privkey_t. 
Typical algo:
1. the user generate a x509_privkey_t 
2. it generates a x509_request_t from its x509_privkey_t
3. it send its x509_request_t to the 'certificate authority'
4. the certificate authority converts it in a x509_cert_t for the user
5. the certificate authority send back the x509_cert_t to the users
6. the user now has a x509_privkey_t and a x509_cert_t authenticated by the 
   certificate authority

*/

/* system include */
/* local include */
#include "neoip_x509_request.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	x509_request_t::zeroing()	throw()
{
	gnutls_request = NULL;
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	x509_request_t::nullify()		throw()
{ 
	if( is_null() )	return;
	if( gnutls_request ){
		gnutls_x509_crq_deinit(gnutls_request);
		gnutls_request = NULL;
	}
}

/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	x509_request_t::copy(const x509_request_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;
	
	gnutls_x509_crq_init(&gnutls_request);
	
// copy thru gnutls serialization as no copy function is provided
// - not thru neoip serialization to avoid chicken/egg issues
	int 		err;
	gnutls_datum	gtls_datum;
	char		data_buf[5*1024];				// TODO to replace
	size_t		data_len = sizeof(data_buf);
	// export the requestificate in DER
	err = gnutls_x509_crq_export( other.gnutls_request, GNUTLS_X509_FMT_DER, data_buf, &data_len );
	if( err < 0 ){
		KLOG_ERR("Error exporting request. error=" << gnutls_strerror(err) );
		DBG_ASSERT( 0 );
	}
	gtls_datum.data = (unsigned char *)data_buf;
	gtls_datum.size = (unsigned int)data_len;
   	// import the requestificate in DER
 	err = gnutls_x509_crq_import(gnutls_request, &gtls_datum, GNUTLS_X509_FMT_DER);
	if( err < 0 ){
		KLOG_ERR("Error exporting request. error=" << gnutls_strerror(err) );
		DBG_ASSERT( 0 );
	}
	
}
	
/** \brief return true if the object is null
 */
bool	x509_request_t::is_null()		const throw()
{
	if( gnutls_request == NULL )	return true;
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the name of this certificate
 * 
 * @return the name of the certificate or "" if an error occured
 */
std::string x509_request_t::common_name()	const throw()
{
	char	buf_data[5*1024];		// TODO to change
	size_t	buf_len	= sizeof(buf_data);
	int	err;
	// get the common name	
	err = gnutls_x509_crq_get_dn_by_oid( gnutls_request, GNUTLS_OID_X520_COMMON_NAME, 0
					, 0, buf_data, &buf_len );
	if( err < 0 ){
		KLOG_ERR("Cant get the common_name from a request. error=" << gnutls_strerror(err) );
		return "";
	}
	// return the just built string
	return std::string(buf_data);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			request generation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief generate a certficate request from a private key and a name
 * 
 * @param name		the name used for this certificate.
 *			the name is stored in the common_name field of the x509 certficiate
 * @param privkey	The private key for which this request is generated
 */
crypto_err_t	x509_request_t::generate(const std::string name, const x509_privkey_t &privkey)	throw()
{
	int	gnutls_err;
	// nullify the object if needed
	if( !is_null() )	nullify();
	// init the gnutls_request
	gnutls_x509_crq_init(&gnutls_request);
	// set the common name
	gnutls_err	= gnutls_x509_crq_set_dn_by_oid(gnutls_request, GNUTLS_OID_X520_COMMON_NAME, 0
								, name.c_str(), name.size());
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// set the version
	gnutls_err	= gnutls_x509_crq_set_version(gnutls_request, 1);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// set the private key
	gnutls_err	= gnutls_x509_crq_set_key(gnutls_request, privkey.gnutls_privkey);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// self sign the certificate request 
	gnutls_err	= gnutls_x509_crq_sign(gnutls_request, privkey.gnutls_privkey);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// return no error
	return crypto_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a x509_request_t into a datum_t in DER format
 */
datum_t	x509_request_t::to_der_datum()				const throw()
{
	char	data_buf[5*1024];				// TODO to replace
	size_t	data_len = sizeof(data_buf);
	// export the x509_request_t in DER formart
	int gnutls_err	= gnutls_x509_crq_export(gnutls_request, GNUTLS_X509_FMT_DER, data_buf, &data_len);
	if( gnutls_err < 0 ){
		KLOG_ERR("Error exporting x509_request_t. error=" << gnutls_strerror(gnutls_err) );
		DBG_ASSERT( 0 );
	}
	// return the just built datum_t
	return datum_t(data_buf, data_len);
}

/** \brief Convert a datum_t in DER format to a x509_request_t
 */
x509_request_t	x509_request_t::from_der_datum(const datum_t &der_datum)		throw()
{
	gnutls_datum	gtls_datum;
	x509_request_t	request;
	// init gtls_datum
	gtls_datum.data = (unsigned char *)der_datum.char_ptr();
	gtls_datum.size = (unsigned int)der_datum.length();
	// init the private key
	gnutls_x509_crq_init(&request.gnutls_request);	
   	// import the x509_request_t from the DER format
    	int gnutls_err	= gnutls_x509_crq_import(request.gnutls_request, &gtls_datum, GNUTLS_X509_FMT_DER);
	if( gnutls_err < 0 ){
		KLOG_ERR("Error importing serialized x509 private due to " << OSTREAMSTR(gnutls_strerror(gnutls_err)));
		return x509_request_t();
	}
	// retrun the just built x509_request_t
	return request;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int x509_request_t::compare( const x509_request_t & other )  const throw()
{
	serial_t	serial_this;
	serial_t	serial_other;	
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// serialize the objects and compare them
	serial_this	<< *this;
	serial_other	<< other;
	return serial_this.compare(serial_other);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string x509_request_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[x509_request";
	oss << " common_name="		<< common_name();
	oss << "]";
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			serialization function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a x509_request_t
 */
serial_t& operator << (serial_t& serial, const x509_request_t &request)		throw()
{
	// serialize the der datum
	serial << request.to_der_datum();
	// return object itself
	return serial;
}

/** \brief unserialize a x509_request_t
 */
serial_t& operator >> (serial_t& serial, x509_request_t &request)			throw(serial_except_t)
{
	datum_t		der_datum;
	// unserialize the datum_t in der format
	serial >> der_datum;
	// convert der_datum inot a x509_request_t
	request	= x509_request_t::from_der_datum(der_datum);
	if( request.is_null() )	throw serial_except_t("Error importing serialized x509_request_t");
	// return the object itself
	return serial;
}

NEOIP_NAMESPACE_END




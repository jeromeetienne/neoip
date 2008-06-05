/*! \file
    \brief Definition of the class \ref x509_cert_t

\par Brief description
This modules handle the x509 certificate parameters.

- TODO all the 'canonical object' stuff is just some old hill-conceived stuff
  from my early day in c++.
  - to remove when time allow

*/

/* system include */
/* local include */
#include "neoip_x509_cert.hpp"
#include "neoip_date.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	x509_cert_t::zeroing()	throw()
{
	gnutls_cert = NULL;
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	x509_cert_t::nullify()		throw()
{ 
	if( is_null() )	return;
	if( gnutls_cert ){
		gnutls_x509_crt_deinit(gnutls_cert);
		gnutls_cert = NULL;
	}
}

/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	x509_cert_t::copy(const x509_cert_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;
	
	gnutls_x509_crt_init(&gnutls_cert);
	
// copy thru gnutls serialization as no copy function is provided
// - not thru neoip serialization to avoid chicken/egg issues
	int 		gnutls_err;
	gnutls_datum	gtls_datum;
	char		data_buf[5*1024];				// TODO to replace
	size_t		data_len = sizeof(data_buf);
	// export the certificate in DER
	gnutls_err	= gnutls_x509_crt_export( other.gnutls_cert, GNUTLS_X509_FMT_DER, data_buf, &data_len );
	if( gnutls_err < 0 ){
		KLOG_ERR("Error exporting cert. gnutls_error=" << gnutls_strerror(gnutls_err) );
		DBG_ASSERT( 0 );
	}
	gtls_datum.data = (unsigned char *)data_buf;
	gtls_datum.size = (unsigned int)data_len;
   	// import the certificate in DER
 	gnutls_err = gnutls_x509_crt_import(gnutls_cert, &gtls_datum, GNUTLS_X509_FMT_DER);
	if( gnutls_err < 0 ){
		KLOG_ERR("Error exporting cert. error=" << gnutls_strerror(gnutls_err) );
		DBG_ASSERT( 0 );
	}
	
}
	
/** \brief return true if the object is null
 */
bool	x509_cert_t::is_null()		const throw()
{
	if( gnutls_cert == NULL )	return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the algorithm name of this certificate
 * 
 * @return the name of the certificate or "" if an error occured
 */
std::string	x509_cert_t::algo_name()	const throw()
{
	size_t	nb_bit	= 0;
	// get the public key algo from gnutls
	int	pk_algo = gnutls_x509_crt_get_pk_algorithm(gnutls_cert, &nb_bit);
	// if an error occured, return std::string()
	if( pk_algo < 0 )	return std::string();
	// else convert it to string
	return gnutls_pk_algorithm_get_name((gnutls_pk_algorithm)pk_algo);
}

/** \brief return the key length in bit
 * 
 * @return the key length >= 0, or -1 if an error occured
 */
size_t	x509_cert_t::key_length()	const throw()
{
	size_t	nb_bit	= 0;
	// get the public key algo from gnutls
	int	pk_algo = gnutls_x509_crt_get_pk_algorithm(gnutls_cert, &nb_bit);
	// if an error occured, return 0
	if( pk_algo < 0 )	return 0;
	// else return the number of bit
	return nb_bit;
}

/** \brief return the subject name of this certificate
 * 
 * @return the name of the certificate or "" if an error occured
 */
std::string	x509_cert_t::subject_name()	const throw()
{
	char	buf_data[5*1024];		// TODO to change
	size_t	buf_len	= sizeof(buf_data);
	int	err;
	// get the common name	
	err = gnutls_x509_crt_get_dn_by_oid( gnutls_cert, GNUTLS_OID_X520_COMMON_NAME, 0
						, 0, buf_data, &buf_len );
	if( err < 0 ){
		KLOG_ERR("Cant get the common_name from a certificate. error=" << gnutls_strerror(err) );
		return "";
	}
	return std::string(buf_data);
}

/** \brief return the issuer name of this certificate
 * 
 * - TODO this function return the whole distingished name while i would like only the 
 *   the common name.
 *   - this will do for now but will likely require change in the future
 * 
 * @return the name or "" if an error occured
 */
std::string	x509_cert_t::issuer_name()	const throw()
{
	char	buf_data[5*1024];		// TODO to change
	size_t	buf_len	= sizeof(buf_data);
	int	err;
	// get the issuer distinguished name
	err = gnutls_x509_crt_get_issuer_dn(gnutls_cert, buf_data, &buf_len);
	if( err < 0 ){
		KLOG_ERR("Cant get the issuer distinguished name from a certificate. error="
							<< gnutls_strerror(err) );
		DBG_ASSERT( 0 );
		return "";
	}
	// KLUDGE: +3 to remove the CN= in front of the name
	// - gnutls add this CN= and i dont know how to get it clean
	return std::string(buf_data+3);
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        certificate GENERATION
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief init parameter in a certificate before generating it
 */
crypto_err_t	x509_cert_t::generate_param( const x509_request_t &cert_req )	throw()
{
	date_t	present		= date_t::present();
	date_t	expiration_date	= present + delay_t::from_sec(30*24*60*60);	// TODO make a define
	int	serial		= 0x12345678;		// TODO what is this ?
	int	gnutls_err;
	// nullify the object if needed
	if( !is_null() )	nullify();
	// init the gnutls_request
	gnutls_x509_crt_init(&gnutls_cert);
		
	// set the certficate request
	gnutls_err	= gnutls_x509_crt_set_crq( gnutls_cert, cert_req.gnutls_request );
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// set the serial number
	gnutls_err	= gnutls_x509_crt_set_serial( gnutls_cert, &serial, sizeof(serial) );
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// set activation time
	gnutls_err	= gnutls_x509_crt_set_activation_time( gnutls_cert, present.to_time_t() );
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// set expiration time
	gnutls_err	= gnutls_x509_crt_set_expiration_time( gnutls_cert, expiration_date.to_time_t() );
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);

#if 1
	unsigned int key_usage = 0;
	key_usage |= GNUTLS_KEY_CRL_SIGN;
	key_usage |= GNUTLS_KEY_KEY_CERT_SIGN;
	key_usage |= GNUTLS_KEY_KEY_AGREEMENT;
	key_usage |= GNUTLS_KEY_KEY_ENCIPHERMENT;
	key_usage |= GNUTLS_KEY_DATA_ENCIPHERMENT;
	key_usage |= GNUTLS_KEY_DIGITAL_SIGNATURE;
	// set the key usage of the generated certificate
	gnutls_err = gnutls_x509_crt_set_key_usage(gnutls_cert, key_usage);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
#endif
	// clear the CA status of the generated certificate
	gnutls_err	= gnutls_x509_crt_set_ca_status(gnutls_cert, 0);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// clear the CA status of the generated certificate
	gnutls_err	= gnutls_x509_crt_set_version(gnutls_cert, 3);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// return no error
	return crypto_err_t::OK;
}

/** \brief generate a certificate from a request
 */
crypto_err_t	x509_cert_t::generate_from_request( const x509_request_t &cert_req
			, const x509_cert_t &ca_cert, const x509_privkey_t &ca_privkey )	throw()
{
	crypto_err_t	crypto_err;
	// generate the parameter
	crypto_err	= generate_param(cert_req);
	if( crypto_err.failed() )	return crypto_err;
	// sign the produced certificate
	int gnutls_err	= gnutls_x509_crt_sign( gnutls_cert, ca_cert.gnutls_cert, ca_privkey.gnutls_privkey );
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// return no error
	return crypto_err_t::OK;
}

/** \brief generate a selfsigned certificate
 */
crypto_err_t	x509_cert_t::generate_selfsigned( const x509_request_t &cert_req
						, const x509_privkey_t &privkey )	throw()
{
	crypto_err_t	crypto_err;
	// generate the parameter
	crypto_err	= generate_param(cert_req);
	if( crypto_err.failed() )	return crypto_err;
	// sign the produced certificate
	int gnutls_err	= gnutls_x509_crt_sign( gnutls_cert, gnutls_cert, privkey.gnutls_privkey );
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	// return no error
	return crypto_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			certificate verification
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief verify the authenticity of a certificate
 * 
 * @return return true if the certificate is valid, false otherwise
 */
crypto_err_t	x509_cert_t::verify_cert(const x509_cert_t &cert)	const throw()
{
	unsigned int	verify_cert;
	// verify the certificate
	int gnutls_err	= gnutls_x509_crt_verify(cert.gnutls_cert, (gnutls_x509_crt_int**)&gnutls_cert, 1
								, 0, &verify_cert);
	if( gnutls_err < 0 )		return crypto_err_from_gnutls(gnutls_err);
   	// if the verify_cert is zero, the cert IS NOT verified
   	if( !verify_cert )	return crypto_err_t(crypto_err_t::ERROR, "verification failed");
   	// return no error as it is verified
   	return crypto_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			data verification
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief verify the signature of a data
 * 
 * @return true if the signature is valid, false otherwise
 */
crypto_err_t	x509_cert_t::verify_data(const void *data_ptr, size_t data_len, const void *signature_ptr
						, size_t signature_len)		const throw()
{
	gnutls_datum	datum_data, datum_sign;
	// init data_datum
	datum_data.data	= (unsigned char *)data_ptr;
	datum_data.size	= (unsigned int)data_len;
	// init datum_sign
	datum_sign.data = (unsigned char *)signature_ptr;
	datum_sign.size = (unsigned int)signature_len;
	// actually verify the signature
	int succeed = gnutls_x509_crt_verify_data(gnutls_cert, 0, &datum_data, &datum_sign);
   	if( !succeed )	return crypto_err_t(crypto_err_t::ERROR, "verification failed");
   	// return noerror as the verification succeed
	return crypto_err_t::OK;   	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a x509_cert_t into a datum_t in DER format
 */
datum_t	x509_cert_t::to_der_datum()				const throw()
{
	char	data_buf[5*1024];				// TODO to replace
	size_t	data_len = sizeof(data_buf);
	// export the x509_cert_t in DER formart
	int err = gnutls_x509_crt_export(gnutls_cert, GNUTLS_X509_FMT_DER, data_buf, &data_len);
	if( err < 0 ){
		KLOG_ERR("Error exporting cert. error=" << gnutls_strerror(err) );
		DBG_ASSERT( 0 );
	}
	// return the just built datum_t
	return datum_t(data_buf, data_len);
}

/** \brief Convert a datum_t in DER format to a x509_cert_t
 */
x509_cert_t	x509_cert_t::from_der_datum(const datum_t &der_datum)		throw()
{
	gnutls_datum	gtls_datum;
	x509_cert_t	cert;
	// init gtls_datum
	gtls_datum.data = (unsigned char *)der_datum.char_ptr();
	gtls_datum.size = (unsigned int)der_datum.length();
	// init the certificate
	gnutls_x509_crt_init(&cert.gnutls_cert);
   	// import the x509_cert_t from the DER format
 	int err = gnutls_x509_crt_import(cert.gnutls_cert, &gtls_datum, GNUTLS_X509_FMT_DER);
	if( err < 0 ){
		KLOG_ERR("Error importing serialized x509 certificate due to " << OSTREAMSTR(gnutls_strerror(err)));
		return x509_cert_t();
	}
	// retrun the just built x509_cert_t
	return cert;
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
int x509_cert_t::compare( const x509_cert_t & other )  const throw()
{
	serial_t	serial_this;
	serial_t	serial_other;	
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// serialize the objects
	serial_this << *this;
	serial_other<< other;
	
	// compare the serial_t
	return serial_this.compare(serial_other);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string x509_cert_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[x509_cert";
	oss << " algo_name="		<< algo_name();
	oss << " key_length="		<< key_length();
	oss << " subject_name="		<< subject_name();
	oss << " issuer_name="		<< issuer_name();
	oss << "]";
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			serialization
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a x509_cert_t
 */
serial_t& operator << (serial_t& serial, const x509_cert_t &cert)		throw()
{
	// serialize the der datum
	serial << cert.to_der_datum();
	// return object itself
	return serial;
}

/** \brief unserialize a x509_cert_t
 */
serial_t& operator >> (serial_t& serial, x509_cert_t &cert)			throw(serial_except_t)
{
	datum_t		der_datum;
	// unserialize the datum_t in der format
	serial >> der_datum;
	// convert der_datum inot a x509_cert_t
	cert	= x509_cert_t::from_der_datum(der_datum);
	if( cert.is_null() )	throw serial_except_t("Error importing serialized x509_cert_t");
	// return the object itself
	return serial;
}

NEOIP_NAMESPACE_END




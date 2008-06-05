/*! \file
    \brief Definition of the class \ref x509_privkey_t

\par Brief description
This modules handle the x509 private keys

*/

/* system include */
/* local include */
#include "neoip_x509_privkey.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       canonical object management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	x509_privkey_t::zeroing()	throw()
{
	gnutls_privkey = NULL;
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	x509_privkey_t::nullify()		throw()
{ 
	if( is_null() )	return;
	if( gnutls_privkey ){
		gnutls_x509_privkey_deinit(gnutls_privkey);
		gnutls_privkey = NULL;
	}
}

/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	x509_privkey_t::copy(const x509_privkey_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;
	
	gnutls_x509_privkey_init(&gnutls_privkey);
	gnutls_x509_privkey_cpy(gnutls_privkey, other.gnutls_privkey);
}
	
/** \brief return true if the object is null
 */
bool	x509_privkey_t::is_null()		const throw()
{
	if( gnutls_privkey == NULL )	return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the algorithm name
 */
std::string	x509_privkey_t::algo_name()	const throw()
{
	// get the public key algo from gnutls
	int	pk_algo = gnutls_x509_privkey_get_pk_algorithm(gnutls_privkey);
	// if an error occured, return std::string()
	if( pk_algo < 0 )	return std::string();
	// else convert it to string
	return gnutls_pk_algorithm_get_name((gnutls_pk_algorithm)pk_algo);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			private key generation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief generate a private key of len_bit
 * 
 * - the private key will be RSA
 * 
 * @param len_bit the number of bit of the private key
 * @return false if no error occured, true otherwise
 */
crypto_err_t x509_privkey_t::generate(size_t len_bit)			throw()
{
	// nullify the object if needed
	if( !is_null() )	nullify();
	// init the private key
	gnutls_x509_privkey_init(&gnutls_privkey);
	// generate the privkey key
	int gnutls_err	= gnutls_x509_privkey_generate(gnutls_privkey, GNUTLS_PK_RSA, len_bit, 0 );
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
	
	// return no error
	return crypto_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        SIGN DATA
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief sign a data and return a datum_t containing the signature
 * 
 * @return a datum_t containing the signature if no error occured, or a null one
 *         if an error occured.
 */
datum_t	x509_privkey_t::sign_data( const void *data_ptr, size_t data_len) const throw()
{
	char		signature[5*1024];	// TODO to change with a define or something
	size_t		signature_len = sizeof(signature);
	gnutls_datum	gtls_datum;
	// init datum
	gtls_datum.data	= (unsigned char *)data_ptr;
	gtls_datum.size	= (unsigned int)data_len;
	// do verify the signature
	int gnutls_err	= gnutls_x509_privkey_sign_data( gnutls_privkey, GNUTLS_DIG_SHA, 0, &gtls_datum
									, signature, &signature_len);
	if( gnutls_err < 0 ){
		KLOG_ERR( "Error signing data with " << *this << " error=" << gnutls_strerror(gnutls_err));
		return datum_t();
	}
	// return the just built signature
	return datum_t(signature, signature_len);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a x509_privkey_t into a datum_t in DER format
 */
datum_t	x509_privkey_t::to_der_datum()				const throw()
{
	char	data_buf[5*1024];				// TODO to replace
	size_t	data_len = sizeof(data_buf);
	// export the x509_privkey_t in DER formart
	int gnutls_err	= gnutls_x509_privkey_export(gnutls_privkey, GNUTLS_X509_FMT_DER, data_buf, &data_len);
	if( gnutls_err < 0 ){
		KLOG_ERR("Error exporting x509_privkey_t. error=" << gnutls_strerror(gnutls_err) );
		DBG_ASSERT( 0 );
	}
	// return the just built datum_t
	return datum_t(data_buf, data_len);
}

/** \brief Convert a datum_t in DER format to a x509_privkey_t
 */
x509_privkey_t	x509_privkey_t::from_der_datum(const datum_t &der_datum)		throw()
{
	gnutls_datum	gtls_datum;
	x509_privkey_t	privkey;
	// init gtls_datum
	gtls_datum.data = (unsigned char *)der_datum.char_ptr();
	gtls_datum.size = (unsigned int)der_datum.length();
	// init the private key
	gnutls_x509_privkey_init(&privkey.gnutls_privkey);	
   	// import the x509_privkey_t from the DER format
 	int gnutls_err	= gnutls_x509_privkey_import(privkey.gnutls_privkey, &gtls_datum, GNUTLS_X509_FMT_DER);
	if( gnutls_err < 0 ){
		KLOG_ERR("Error importing serialized x509_privkey_t due to " << OSTREAMSTR(gnutls_strerror(gnutls_err)));
		return x509_privkey_t();
	}
	// retrun the just built x509_privkey_t
	return privkey;
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
int x509_privkey_t::compare(const x509_privkey_t & other)		const throw()
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
std::string x509_privkey_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[x509_privkey";
	oss << " algo_name="		<< algo_name();
	oss << "]";
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			serialization
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a x509_privkey_t
 */
serial_t& operator << (serial_t& serial, const x509_privkey_t &privkey)		throw()
{
	// serialize the der datum
	serial << privkey.to_der_datum();
	// return object itself
	return serial;
}

/** \brief unserialize a x509_privkey_t
 */
serial_t& operator >> (serial_t& serial, x509_privkey_t &privkey)		throw(serial_except_t)
{
	datum_t		der_datum;
	// unserialize the datum_t in der format
	serial >> der_datum;
	// convert der_datum inot a x509_privkey_t
	privkey	= x509_privkey_t::from_der_datum(der_datum);
	if( privkey.is_null() )	throw serial_except_t("Error importing serialized x509_privkey_t");
	// return the object itself
	return serial;
}

NEOIP_NAMESPACE_END




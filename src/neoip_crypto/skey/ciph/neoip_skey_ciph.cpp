/*! \file
    \brief Definition of the class \ref skey_ciph_t

\par Brief description
\ref neoip_crypto_skey_ciph provides symmetric algorithms (e.g. AES, DES etc...) 
for encryption and decryption. The mode is flexible too (e.g. CBC, CTR etc...)

\par Go away From Gcrypt
- i have strong dubt about the gcrypt library
  -# poor error handling
     - if you pass an bad parameter, it dump a undescriptive message and exit()!!!
       so it leave you no way to know where the message comes from and even less
       what trigger it
  -# weird memory support
     - the secured memory isnt dynamically allocated and require central init!!!
     - moreover if you need more than the initialy allocated pool, stuff are failling
       Thus it limit uselessly the scalability of the library
     - btw my way to handle this is crap at best
- currently the skey_ciph and skey_ciph are coded on top of it 
  - as the API is well abstracted, to remove gcrypt could be done later
  - i dont know for newer code... like pkey...
  - another advantage of the gnutls+gcrypt is that i already have a working
    code using it in the C part... so a simple porting would do. 
  - if i choose to replace them, i dont know which library to use...
    - so time to choose
    - and time to learn it...

*/

/* system include */
/* local include */
#include "neoip_skey_ciph.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	skey_ciph_t::zeroing()	throw()
{
	ciph_type	= skey_ciph_type_t();	
	// nothing about the gcrypt lib context because nothing in gcrypt seems to provide this function
	// - care is taken not to rely on its value
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	skey_ciph_t::nullify()		throw()
{ 
	if( is_null() )	return;
	ciph_type	= skey_ciph_type_t();
	// close the gcrypt mac context
// TODO	deinit_gcry_md_hd();
}


/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	skey_ciph_t::copy(const skey_ciph_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;

	ciph_type	= other.ciph_type;	
	if( init_gcry_hd() ){
		this->ciph_type	= skey_ciph_type_t();
		return;
	}
}
	
/** \brief return true if the object is null
 */
bool	skey_ciph_t::is_null()		const throw()
{
	if( ciph_type.is_null() )	return true;
	return false;
}

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int skey_ciph_t::compare( const skey_ciph_t & other )  const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the type
	return ciph_type.compare(other.ciph_type);
}


/** \brief convert the object into a string
 */
std::string skey_ciph_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return ciph_type.to_string();
}

/** \brief constructor based on a value
 */
skey_ciph_t::skey_ciph_t(const skey_ciph_type_t &ciph_type)			throw()
{
	zeroing();
	this->ciph_type	= ciph_type;
	if( init_gcry_hd() ){
		this->ciph_type	= skey_ciph_type_t();
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         init/deinit gcrypt context
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief init the gcry_md_hd
 * 
 * - this init is assumed to be done everytime it is done for ciph_type
 */
bool skey_ciph_t::init_gcry_hd()				throw()
{
	const skey_ciph_algo_t &algo	= ciph_type.get_algo();
	// get the algo from the algo_str
	int gcry_cipher_algo = gcry_cipher_map_name( algo.to_string().c_str() );
	// if the algo_str isnt supported, throw an exception
	if( !gcry_cipher_algo ){
		KLOG_ERR("Can't map " << algo.to_string() << " in gcrypt cipher_map_name");
		return true;
	}
	// open the gcrypt cipher
	if( gcry_cipher_open(&gcry_cipher_hd, gcry_cipher_algo, get_gcry_cipher_mode(), GCRY_CIPHER_SECURE) ){
		KLOG_ERR("Can't open " << ciph_type << " in gcrypt cipher_open");
		return true;
	}
	// return false if no error occured
	return false;
}

/** \brief deinit the gcry_md_hd
 * 
 * - this deinit is assumed to be done everytime it is done for ciph_type
 */
void skey_ciph_t::deinit_gcry_hd()						throw()
{
	// close the gcrypt ciph context
	gcry_cipher_close( gcry_cipher_hd );
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            mode auxilary function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true if the mode require padding, false otherwise
 */
bool	skey_ciph_t::require_padding()				const throw()
{
	switch(ciph_type.get_mode().get_value()){
	// list of unpadded modes
	case skey_ciph_mode_t::CTR:	return false;
	case skey_ciph_mode_t::STREAM:	return false;
	// list of padded moded
	case skey_ciph_mode_t::ECB:	// fall through	
	case skey_ciph_mode_t::CBC:	return true;
	default:	DBG_ASSERT( 0 );
	}
	// TODO this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;
}

/** \brief return true if the mode require an IV, false otherwise
 */
bool	skey_ciph_t::require_iv()				const throw()
{
	switch(ciph_type.get_mode().get_value()){
	// list of modes NOT requiring iv
	case skey_ciph_mode_t::ECB:	return false;
	case skey_ciph_mode_t::STREAM:	return false;
	// list of modes requiring iv
	case skey_ciph_mode_t::CTR:	return true;
	case skey_ciph_mode_t::CBC:	return true;
	default:	DBG_ASSERT( 0 );
	}	
	// TODO this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                constant convertion between gcrypt and neoip
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/** \brief return the gcry_cipher_mode
 */
int	skey_ciph_t::get_gcry_cipher_mode()			const throw()
{
	switch(ciph_type.get_mode().get_value()){
	case skey_ciph_mode_t::ECB:	return GCRY_CIPHER_MODE_ECB;
	case skey_ciph_mode_t::CTR:	return GCRY_CIPHER_MODE_CTR;
	case skey_ciph_mode_t::CBC:	return GCRY_CIPHER_MODE_CBC;
	case skey_ciph_mode_t::STREAM:	return GCRY_CIPHER_MODE_STREAM;
	default:	DBG_ASSERT( 0 );
	}
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return 0;
}

/** \brief return the gcrypt algo id
 */
int	skey_ciph_t::get_gcry_cipher_algo()			const throw()
{
	const skey_ciph_algo_t &algo		= ciph_type.get_algo();
	int 			gcry_cipher_algo= gcry_cipher_map_name( algo.to_string().c_str() );	
	return gcry_cipher_algo;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         PADDING functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the padding ala pkcs5
 * 
 * - see rfc2898.6.1.1.4
 */
void	skey_ciph_t::padding_build(size_t src_len, void *dst_ptr_param ) const throw()
{
	uint8_t	*		dst_ptr		= (uint8_t *)dst_ptr_param;
	size_t			padding_len	= padding_cpu_len(src_len);
	// if no padding is required, return now
	if( require_padding() == false )		return;
	// build the padding aka padding_len byte all set to uint8(padding_len)
	for( size_t i = src_len; i < src_len + padding_len; i++ )
		dst_ptr[i] = padding_len;
}

/** \brief parse the padding
 * 
 * - see rfc2898.6.1.1.4
 * 
 * @return the padding length, or -1 if the padding is invalid
 */
ssize_t	skey_ciph_t::padding_parse( const void *src_ptr_param, size_t src_len )
								const throw()
{
	const uint8_t *		src_ptr	= (uint8_t *)src_ptr_param;
	// if no padding is required, return 0 now
	if( require_padding() == false )		return	0;

	// sanity check - src_len MUST be at least 1 byte
	// - NOTE it isnt an actual limitation as pkcs5 padding is always >= 1 byte long
	DBG_ASSERT( src_len > 0 );

	// get the padding length
	size_t	padding_len	= src_ptr[src_len-1];

	// check if the source buffer is big enougth to contain the padding
	if( src_len < padding_len )	return -1;
	// check if all the byte of the padding are all equal to the padding length
	for( size_t i = 1; i < padding_len; i++ ){
		if( src_ptr[src_len-1-i] != (unsigned char)padding_len )
			return -1;
	}
	// return the padding length
	return padding_len;
}

/** \brief return the padding length (ala pkcs5)
 * 
 * - see rfc2898.6.1.1.4
 * - if the mode require padding, the padding is ALWAYS at least 1 byte
 */
size_t	skey_ciph_t::padding_cpu_len(size_t plaintxt_len)	const throw()
{
	// if no padding is required, return 0
	if( require_padding() == false )		return 0;
	// compute and return the padding length
	size_t	block_len	= get_block_len();
	size_t	padding_len	= block_len - (plaintxt_len % block_len);
	// sanity check - because of the current padding encoding, the padding length MUST < 255
	DBG_ASSERT(padding_len < 255);
	// return the length
	return padding_len;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         UTILITY function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the block length
 */
size_t	skey_ciph_t::get_block_len()				const throw()
{
	size_t			block_len;
	// get the block length
	if( gcry_cipher_algo_info(get_gcry_cipher_algo(), GCRYCTL_GET_BLKLEN, NULL, &block_len) )
		DBG_ASSERT( 0 );
	return block_len;		
}

/** \brief return the key length
 */
size_t	skey_ciph_t::get_key_len()				const throw()
{
	size_t			key_len;
	// get the block length
	if( gcry_cipher_algo_info( get_gcry_cipher_algo(), GCRYCTL_GET_KEYLEN, NULL, &key_len ) )
		DBG_ASSERT( 0 );
	return key_len;		
}

/** \brief return the length of the cipher text that gonna be produced by this plaintxt_len
 */
size_t	skey_ciph_t::get_ciphertxt_len(size_t plaintxt_len)	const throw()
{
	return plaintxt_len + padding_cpu_len(plaintxt_len);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         ENCIPHER functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the IV
 */
void	skey_ciph_t::set_iv(const skey_ciph_iv_t & iv)				const throw()
{
	void *			iv_ptr = (void *)iv.get_iv_ptr();
	gcry_error_t		err;
	// if no iv is required, return now
	if( require_iv() == false )		return;
	// if an iv is required, set it
	if( ciph_type.get_mode() == skey_ciph_mode_t::CTR ){
		err = gcry_cipher_setctr( gcry_cipher_hd, iv_ptr, get_block_len() );
		DBG_ASSERT( !err );
	}else{
		err = gcry_cipher_setiv( gcry_cipher_hd, iv_ptr, get_block_len() );
		DBG_ASSERT( !err );
	}
}

/** \brief initialize the key
 *
 * @param key_len the key length.
 *                it MUST be equal to the block length.
 * @return false if the key is valid, true if an error occured
 */
bool	skey_ciph_t::init_key(const void *key_ptr, size_t key_len)	throw()
{
	// sanity check - the key_len MUST be equal to the key_len
	// TODO - removed it because it caused issue with rc4
	// - seems like it would apply only to block mode 
	//DBG_ASSERT( key_len == get_key_len() );
	// copy the key data
	key_datum = datum_t(key_ptr, key_len, datum_t::SECMEM);
	// set the gcrypt key	
	if( gcry_cipher_setkey(gcry_cipher_hd, key_datum.get_data(), key_datum.get_len()) )
		return true;
	return false;
}


/** \brief encipher data
 * 
 * @param dst_len The destination buffer length.
 *                dst_len MUST be equal to src_len + get_padding_length(src_len) 
 */
void	skey_ciph_t::encipher(const skey_ciph_iv_t & iv, const void *src_ptr, size_t src_len
						, void *dst_ptr, size_t dst_len)	throw()
{
	// sanity check - dst_len MUST be equal ::get_ciphertxt_len(src_len)
	DBG_ASSERT( dst_len == get_ciphertxt_len(src_len) );
	// sanity check - the key_data MUST be initialized, so init_key() MUST have been called
	DBG_ASSERT( !key_datum.is_null() );
	// copy the src buffer to the destination
	DBG_ASSERT( dst_len >= src_len );
	// TODO here to do that IIF src_ptr != dst_ptr
	memcpy( dst_ptr, src_ptr, src_len );
	// build the padding
	padding_build( src_len, dst_ptr );
	// set the iv
	set_iv( iv );
	// Now Do encipher the data
	if( gcry_cipher_encrypt(gcry_cipher_hd, (unsigned char *)dst_ptr, dst_len, NULL, 0) )
		DBG_ASSERT( 0 );
}


/** \brief Decipher Data
 * 
 * @param dst_len The destination buffer length.
 *                It MUST be greated than or equal to src_len.
 * @return the plain text length used in the destination buffer, or -1 if an
 *         error occured (e.g. invalid padding in the resulting plain text)
 */
ssize_t	skey_ciph_t::decipher( const skey_ciph_iv_t & iv, const void *src_ptr
						, size_t src_len, void *dst_ptr, size_t dst_len )
						throw()
{
	// sanity check - the key_data MUST be initialized, so init_key() MUST have been called
	DBG_ASSERT( !key_datum.is_null() );
	// copy the src buffer to the destination
	DBG_ASSERT( dst_len >= src_len );
	memcpy( dst_ptr, src_ptr, src_len );
	// set the iv
	set_iv( iv );
	// does the encrypt itself
	gcry_error_t	err;
	err = gcry_cipher_decrypt(gcry_cipher_hd, (unsigned char *)dst_ptr, src_len, NULL, 0 );
	if( err ){
		KLOG_ERR("gcry_cipher_decrypt failed due to " << gcry_strerror(err) );
		return -1;
	}
	// parse the padding
	ssize_t	padding_len = padding_parse( dst_ptr, src_len );
	// if the padding is invalid, return -1
	if( padding_len == -1 )	return -1;
	// return the plain text length aka src_len - padding_len
	return src_len - padding_len;
}



NEOIP_NAMESPACE_END


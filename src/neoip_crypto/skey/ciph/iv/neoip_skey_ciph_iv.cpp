/*! \file
    \brief Definition of the \ref neoip_skey_ciph_iv class

*/

/* system include */
/* local include */
#include "neoip_skey_ciph_iv.hpp"
#include "neoip_skey_ciph.hpp"
#include "neoip_assert.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                               CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
skey_ciph_iv_t::skey_ciph_iv_t(skey_ciph_t *skey_ciph)			throw()
{
	// set the backpointer
	this->skey_ciph	= skey_ciph;
}

/** \brief destructor
 */
skey_ciph_iv_t::~skey_ciph_iv_t()						throw()
{
	// close the gcrypt cipher context
	if( !key_datum.is_null() )		gcry_cipher_close(gcry_cipher_hd);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                             UTILITY FUNCTION
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the iv based on data provided by the 
 * @param src_len the length of the source data.
 *                This length MUST be <= than the cipher block length.
 */
void skey_ciph_iv_t::set_iv_data(const void *src_ptr, size_t src_len)	throw()
{
	size_t		block_len	= skey_ciph->get_block_len();
	// if the mode do not require IV, return now
	if( skey_ciph->require_iv() == false )	return;
	// sanity check - the src_len MUST be <= block_len
	DBG_ASSERT( src_len <= block_len );
	// if the src_len == the block_len, dont modify the IV
	if( src_len == block_len ){
		iv_datum = datum_t(src_ptr, src_len);
		return;
	}
// if the src_len is <= block_len, build the IV to the source encrypted in ECB with the algo
	// if the key_data is NULL, the crypto ecb stuff isnt yet init. So init it
	if( key_datum.is_null() ){
		int	gcry_cipher_algo = skey_ciph->get_gcry_cipher_algo();
		// open the gcrypt cipher in ECB
		if(gcry_cipher_open(&gcry_cipher_hd, gcry_cipher_algo, GCRY_CIPHER_MODE_ECB, GCRY_CIPHER_SECURE)){
			KLOG_ERR( "skey_ciph_iv_t cant open algo for " + OSTREAMSTR(*skey_ciph) );
			EXP_ASSERT( 0 );
		}
	}
	// the key must be already initialize in the skey_ciph_t
	DBG_ASSERT( !skey_ciph->key_datum.is_null() );
	// if the local copy of the key is different than the skey_ciph_t one, init the key 
	// - there to handle key change in the main
	if( key_datum != skey_ciph->key_datum ){
		key_datum = skey_ciph->key_datum;
		// setup the key in gcrypt
		if( gcry_cipher_setkey(gcry_cipher_hd, key_datum.get_data(), key_datum.get_len()) )
			DBG_ASSERT( 0 );
	}
	// init the iv source data
	iv_datum = datum_t(block_len);
	memset( iv_datum.get_data(), 0, iv_datum.get_len() );
	memcpy( iv_datum.get_data(), src_ptr, src_len );	
	// Do encrypt the iv source data with the same algo in ECB to get the real IV
	if( gcry_cipher_encrypt(gcry_cipher_hd, (unsigned char *)iv_datum.get_data(), iv_datum.get_len(), NULL, 0) )
		DBG_ASSERT( 0 );
}

/** \brief get the iv
 */
const void *skey_ciph_iv_t::get_iv_ptr()	const throw()
{
	return iv_datum.get_data();
}

NEOIP_NAMESPACE_END

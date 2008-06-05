/*! \file
    \brief Header of the \ref neoip_crypto_skey_ciph_iv.cpp

*/


#ifndef __NEOIP_SKEY_CIPH_IV_HPP__ 
#define __NEOIP_SKEY_CIPH_IV_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_crypto_gcrypt.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class skey_ciph_t;

/** \brief class to handle the iv
 * 
 */
class skey_ciph_iv_t {
private:
	skey_ciph_t *		skey_ciph;	//!< backpointer on the skey_ciph_t 
	datum_t			iv_datum;	//!< pointer on the last computed iv.
						//!< the length is the block length of the cipher algo.

	// trick to autogenerate the IV thru a ECB mode of the skey_ciph_t algo
	gcry_cipher_hd_t	gcry_cipher_hd;	//!< gcrypt context
	datum_t			key_datum;	//!< store the current key
public:
	// ctor/dtor
	skey_ciph_iv_t(skey_ciph_t *skey_ciph)	throw();
	~skey_ciph_iv_t()			throw();

	// utility function
	void 		set_iv_data(const void *src_ptr, size_t src_len)	throw();
	const void *	get_iv_ptr()						const throw();

friend	class skey_ciph_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_CIPH_IV_HPP__  */




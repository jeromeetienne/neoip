/*! \file
    \brief Definition of the class \ref neoip_random_pool

\par Brief description
This modules handle a random pool

*/

/* system include */
/* local include */
#include "neoip_crypto_gcrypt.hpp"
#include "neoip_random_pool.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief read some random data
 */
void	random_pool_t::read(void *data_ptr, size_t data_len, rndlevel_t rndlevel )	throw()
{
	unsigned	char *data = (unsigned char *)data_ptr;
	switch( rndlevel ){
	case random_pool_t::WEAK:	gcry_create_nonce(data, data_len);
					break;
	case random_pool_t::NORMAL:	gcry_randomize(data, data_len, GCRY_STRONG_RANDOM);
					break;
	case random_pool_t::STRONG:	gcry_randomize(data, data_len, GCRY_VERY_STRONG_RANDOM);
					break;
	default:			DBG_ASSERT( 0 );
	}
}

NEOIP_NAMESPACE_END

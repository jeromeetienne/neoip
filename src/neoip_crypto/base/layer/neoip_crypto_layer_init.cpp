/*! \file
    \brief Implementation of the crypto_layer_t

\par Brief Description
- This source provides the global initialization for the \ref crypto_layer_t
 
*/

/* system include */
#include <gnutls/gnutls.h>
/* local include */
#include "neoip_crypto_layer_init.hpp"
#include "neoip_assert.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

static int crypto_layer_init_counter = 0;

/** \brief init the crypto layer
 */
void crypto_layer_init() throw()
{
	// if it is the first crypto_layer_init, do the init
	if( crypto_layer_init_counter == 0 ){
		// init gnutls
		gnutls_global_init();
#if 0
		// magic init of lib gcrypt
		// - now it is required and it is UNDOCUMENTED!!!! cant believe this
		// - btw if you dont do it the gcrypt lib stop with a 
		//   "operation is not possible without initialized secure memory"
		//   and then do exit() !!!!! so no core, no error returned, nothing!!!!
		if( gcry_control( GCRYCTL_ANY_INITIALIZATION_P ) == 0 ){
			// init 64kbyte of secure memory 
			// - if you need more, you are screwed
			gcry_control( GCRYCTL_INIT_SECMEM, 64*1024, 0 );
			gcry_control( GCRYCTL_INITIALIZATION_FINISHED, 0 );
		}
#endif	
	}
	crypto_layer_init_counter ++;
}

/** \brief deinit the crypto layer
 */
void crypto_layer_deinit() throw()
{
	// sanity check - the init counter MUST be > 0
	DBG_ASSERT( crypto_layer_init_counter > 0 );
	// decrese the crypto layer counter
	crypto_layer_init_counter --;
	// if it is the last crypto_layer_deinit, do the deinit
	if( crypto_layer_init_counter == 0 ){
		// deinit gnutls
		gnutls_global_deinit();
	}
}

NEOIP_NAMESPACE_END





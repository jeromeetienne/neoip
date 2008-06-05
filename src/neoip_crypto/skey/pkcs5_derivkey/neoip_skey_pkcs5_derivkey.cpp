/*! \file
    \brief definition of \ref pkcs5_derivkey function

*/

/* system include */
/* local include */
#include "neoip_skey_pkcs5_derivkey.hpp"
#include "neoip_skey_auth.hpp"
#include "neoip_assert.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief derive a base key using a salt and a iteration count
 *         using pkcs5 first method (rfc2898.5.1)
 */
void skey_pkcs5_derivkey( const skey_auth_type_t &auth_type
				, void *derived_key_ptr		, size_t derived_key_len
				, const void *base_key_ptr	, size_t base_key_len
				, const void *salt_ptr		, size_t salt_len
				, int iteration_count )
{
	// init the hash function
	skey_auth_t	skey_auth(auth_type);
	// sanity check - the skey_auth MUST be NOKEY
	DBG_ASSERT( auth_type.get_mode() == skey_auth_mode_t::NOKEY );
	// santiy check - the skey_auth output length MUST be <= to the derived_key_len
	DBG_ASSERT( derived_key_len <= skey_auth.get_output_len() );

	// allocate secure memory for the temporary buffer
	size_t	tmp_buf_len	= skey_auth.get_output_len();
	void *	tmp_buf_ptr	= nipmem_malloc_sec(tmp_buf_len);

	// compute the first iteration
	skey_auth.init_key( NULL, 0 );
	skey_auth.update( base_key_ptr, base_key_len );
	skey_auth.update( salt_ptr, salt_len );
	memcpy( tmp_buf_ptr, skey_auth.final(), tmp_buf_len );
	/* do icount iterations */
	for( int i = 2; i < iteration_count; i++ ){
		skey_auth.init_key( NULL, 0 );
		skey_auth.update( derived_key_ptr, derived_key_len );
		memcpy( tmp_buf_ptr, skey_auth.final(), tmp_buf_len );
	}
	// copy the final result in the derived_key_ptr
	memcpy( derived_key_ptr, tmp_buf_ptr, derived_key_len );
	
	// free the temporary buffer
	nipmem_free_sec(tmp_buf_ptr, tmp_buf_len);
}

NEOIP_NAMESPACE_END







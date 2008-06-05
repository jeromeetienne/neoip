/*! \file
    \brief Header of the \ref neoip_crypto_pkcs5_derivkey_utest.cpp

*/


#ifndef __NEOIP_SKEY_PKCS5_DERIVKEY_HPP__ 
#define __NEOIP_SKEY_PKCS5_DERIVKEY_HPP__ 

/* system include */
/* local include */
#include "neoip_skey_auth_type.hpp"
#include "neoip_datum.hpp"

NEOIP_NAMESPACE_BEGIN;

void skey_pkcs5_derivkey( const skey_auth_type_t &auth_type
				, void *derived_key_ptr		, size_t derived_key_len
				, const void *base_key_ptr	, size_t base_key_len
				, const void *salt_ptr		, size_t salt_len
				, int iteration_count );


/** \brief helper to pkcs5_derivkey using \ref datum_t
 */
inline void skey_pkcs5_derivkey( const skey_auth_type_t &auth_type, datum_t &derived_key
					, const datum_t base_key
					, const datum_t salt, int iteration_count )
{
	skey_pkcs5_derivkey( auth_type	, derived_key.get_data()	, derived_key.get_len()
					, base_key.get_data()		, base_key.get_len()
					, salt.get_data()		, salt.get_len()
					, iteration_count );
}


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_PKCS5_DERIVKEY_UTEST_HPP__  */




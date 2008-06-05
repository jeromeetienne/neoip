/*! \file
    \brief Definition of the unit test for the \ref pkcs5_derivkey_t and co

\par Brief description
This module performs an unit test for the \ref pkcs5_derivkey_t classes

*/

/* system include */
#include <gnutls/gnutls.h>

/* local include */
#include "neoip_skey_pkcs5_derivkey_utest.hpp"
#include "neoip_skey_pkcs5_derivkey.hpp"
#include "neoip_datum.hpp"
#include "neoip_log.hpp"

using namespace neoip;
/** \brief unit test for the neoip_crypto_skey_pkcs5_derivkey class
 */
int neoip_skey_pkcs5_derivkey_utest()
{
	int				n_error = 0;
	uint8_t				derived_key[20];
	skey_pkcs5_derivkey( skey_auth_type_t("sha1/nokey/20")
			, derived_key, sizeof(derived_key)
			, "bonjour", 7
			, "slota", 5
			, 1000 );
	KLOG_ERR("derived key is " << datum_t(derived_key, sizeof(derived_key), datum_t::NOCOPY) );
		
	if( n_error )	goto error;
	KLOG_ERR("PKCS5_DERIVKEY_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("PKCS5_DERIVKEY_UTEST FAILED!!!!");
	return -1;	
}


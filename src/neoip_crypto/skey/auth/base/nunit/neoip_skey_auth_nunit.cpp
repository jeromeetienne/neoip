/*! \file
    \brief unit test for the skey_auth_t unit test

*/

/* system include */
/* local include */
#include "neoip_skey_auth_nunit.hpp"
#include "neoip_skey_auth.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test auth_type_t
 */
nunit_res_t	skey_auth_testclass_t::auth_type(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// test skey_auth_type_t serialization
	serial_t		serial;
	skey_auth_type_t	auth_type = "md5/hmac/12";
	skey_auth_type_t	auth_type_unserial;	
	serial << auth_type;
	serial >> auth_type_unserial;
	NUNIT_ASSERT( auth_type == auth_type_unserial );
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test md5 hmac
 * 
 * - test vector from rfc2104.A
 */
nunit_res_t	skey_auth_testclass_t::md5_hmac(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	skey_auth_t	skey_auth("md5/hmac/16");
	int		result;
	NUNIT_ASSERT( skey_auth.get_output_len() == 16 );		
	// test vector 1
	skey_auth.init_key("\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb", 16 );
	skey_auth.update( "Hi There", 8 );
	result = memcmp("\x92\x94\x72\x7a\x36\x38\xbb\x1c\x13\xf4\x8e\xf8\x15\x8b\xfc\x9d"
						,skey_auth.final(), skey_auth.get_output_len());
	NUNIT_ASSERT( !result );
	
	// test vector 2
	skey_auth.init_key("Jefe", 4);
	skey_auth.update( "what do ya want for nothing?", 28 );
	result = memcmp("\x75\x0c\x78\x3e\x6a\xb0\xb5\x03\xea\xa8\x6e\x31\x0a\x5d\xb7\x38"
						,skey_auth.final(), skey_auth.get_output_len());
	NUNIT_ASSERT( !result );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test md5 nokey
 * 
 * - test vector from rfc1321.A.5
 */
nunit_res_t	skey_auth_testclass_t::md5_nokey(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	skey_auth_t	skey_auth("md5/nokey/16");
	char		*testvect[]={
	  "\xd4\x1d\x8c\xd9\x8f\x00\xb2\x04\xe9\x80\x09\x98\xec\xf8\x42\x7e", ""
	, "\x0c\xc1\x75\xb9\xc0\xf1\xb6\xa8\x31\xc3\x99\xe2\x69\x77\x26\x61", "a"
	, "\x90\x01\x50\x98\x3c\xd2\x4f\xb0\xd6\x96\x3f\x7d\x28\xe1\x7f\x72", "abc"
	, "\xf9\x6b\x69\x7d\x7c\xb7\x93\x8d\x52\x5a\x2f\x31\xaa\xf1\x61\xd0", "message digest"
	, "\xc3\xfc\xd3\xd7\x61\x92\xe4\x00\x7d\xfb\x49\x6c\xca\x67\xe1\x3b", "abcdefghijklmnopqrstuvwxyz"
	, "\xd1\x74\xab\x98\xd2\x77\xd9\xf5\xa5\x61\x1c\x2c\x9f\x41\x9d\x9f", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
	, "\x57\xed\xf4\xa2\x2b\xe3\xc9\x55\xac\x49\xda\x2e\x21\x07\xb6\x7a", "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
	, NULL};
	// sanity check
	NUNIT_ASSERT( skey_auth.get_output_len() == 16 );
	// test vectors
	for( int i = 0; testvect[i]; i += 2 ){
		int	failed;
		// init the key
		failed = skey_auth.init_key( NULL, 0 );
		NUNIT_ASSERT( !failed );
		// compute the skey_auth
		skey_auth.update( testvect[i+1], strlen(testvect[i+1]) );
		// compare the result
		failed = memcmp(testvect[i],skey_auth.final(), skey_auth.get_output_len());
		NUNIT_ASSERT( !failed );
	}

	// return no error
	return NUNIT_RES_OK;
}


/** \brief test sha1 nokey
 * 
 * - test vector from rfc3174.7.3 and some from http://en.wikipedia.org/wiki/SHA
 */
nunit_res_t	skey_auth_testclass_t::sha1_nokey(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	skey_auth_t	skey_auth("sha1/nokey/20");
	int		failed;
	char		*testvect[]={
	// from rfc3174.7.3
	  "\xA9\x99\x3E\x36\x47\x06\x81\x6A\xBA\x3E\x25\x71\x78\x50\xC2\x6C\x9C\xD0\xD8\x9D", "abc"
	, "\x84\x98\x3E\x44\x1C\x3B\xD2\x6E\xBA\xAE\x4A\xA1\xF9\x51\x29\xE5\xE5\x46\x70\xF1", "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
	// from http://en.wikipedia.org/wiki/SHA
	, "\x2f\xd4\xe1\xc6\x7a\x2d\x28\xfc\xed\x84\x9e\xe1\xbb\x76\xe7\x39\x1b\x93\xeb\x12", "The quick brown fox jumps over the lazy dog"
	, "\xda\x39\xa3\xee\x5e\x6b\x4b\x0d\x32\x55\xbf\xef\x95\x60\x18\x90\xaf\xd8\x07\x09", ""
	, NULL};
	// sanity check
	NUNIT_ASSERT( skey_auth.get_output_len() == 20 );

	// test vectors
	for(size_t i = 0; testvect[i]; i += 2){
		// init the key
		failed = skey_auth.init_key(NULL, 0);
		NUNIT_ASSERT( !failed );
		// compute the skey_auth
		skey_auth.update(testvect[i+1], strlen(testvect[i+1]));
		// compare the result
		failed = memcmp(testvect[i], skey_auth.final(), skey_auth.get_output_len());
		NUNIT_ASSERT( !failed );
	}

	// do the test3 vector of rfc3174.7.3
	// - 1000000 times "a"
	void *	data3	= nipmem_alloca(1000000);
	memset(data3, 'a', 1000000);
	failed = skey_auth.init_key(NULL, 0);
	NUNIT_ASSERT( !failed );
	skey_auth.update( data3, 1000000 );
	NUNIT_ASSERT( skey_auth.get_output() == datum_t("\x34\xAA\x97\x3C\xD4\xC4\xDA\xA4\xF6\x1E\xEB\x2B\xDB\xAD\x27\x31\x65\x34\x01\x6F") );

	// do the test4 vector of rfc3174.7.3
	// - 10 times "0123456701234567012345670123456701234567012345670123456701234567"
	datum_t	data4;
	for(size_t i = 0; i < 10; i++)
		data4 += datum_t("0123456701234567012345670123456701234567012345670123456701234567");
	failed = skey_auth.init_key(NULL, 0);
	NUNIT_ASSERT( !failed );
	skey_auth.update( data4 );
	NUNIT_ASSERT( skey_auth.get_output() == datum_t("\xDE\xA3\x56\xA2\xCD\xDD\x90\xC7\xA7\xEC\xED\xC5\xEB\xB5\x63\x93\x4F\x46\x04\x52") );

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

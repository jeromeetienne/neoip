/*! \file
    \brief unit test for the diffie-hellman unit test

*/

/* system include */
/* local include */
#include "neoip_skey_ciph_nunit.hpp"
#include "neoip_skey_ciph.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


struct neoip_skey_ciph_utest_vector_t {
	const void *	key_ptr;
	int		key_len;
	const void *	iv_ptr;	
	int 		iv_len;
	const void *	plaintxt_ptr;	
	int 		plaintxt_len;
	const void *	ciphertxt_ptr;	
	int 		ciphertxt_len;
};

/** \brief unit test for a given test vector for neoip_skey_ciph
 */
static bool neoip_skey_ciph_utest_one( skey_ciph_t & skey_ciph
					, const std::string label
					, struct neoip_skey_ciph_utest_vector_t &tvect
					)
{
	skey_ciph_iv_t	iv( &skey_ciph );
	size_t		tmp_ciphertxt_len	= skey_ciph.get_ciphertxt_len(tvect.plaintxt_len);
	void *		tmp_ciphertxt		= nipmem_alloca(tmp_ciphertxt_len);
	void *		tmp_plaintxt		= nipmem_alloca(tmp_ciphertxt_len);
	// some logging
	KLOG_DBG( label << " test vector key is "	<< datum_t(tvect.key_ptr, tvect.key_len) );
	KLOG_DBG( label << " test vector iv is "	<< datum_t(tvect.iv_ptr, tvect.iv_len) );
	KLOG_DBG( label << " test vector plaintxt is "	<< datum_t(tvect.plaintxt_ptr, tvect.plaintxt_len) );
	KLOG_DBG( label << " test vector ciphertxt is "	<< datum_t(tvect.ciphertxt_ptr, tvect.ciphertxt_len) );
	// init the key
	bool	invalid_key = skey_ciph.init_key( tvect.key_ptr, tvect.key_len );
	// check the ::init_key return code
	if( invalid_key ){
		KLOG_ERR( label << " test vector failed in init_key");
		return true;
	}
	// setup the iv
	iv.set_iv_data( tvect.iv_ptr, tvect.iv_len );
	// encypher the data
	skey_ciph.encipher( iv, tvect.plaintxt_ptr, tvect.plaintxt_len, tmp_ciphertxt, tmp_ciphertxt_len);
	// check if the enciphered data is equal to the testvector
	if( memcmp( tvect.ciphertxt_ptr, tmp_ciphertxt, tvect.ciphertxt_len) ){
		KLOG_ERR( label << " test vector failed in encryption");
		return true;
	}

	// reinit the key - require when using a stream cipher - they got internal context
	invalid_key = skey_ciph.init_key( tvect.key_ptr, tvect.key_len );
	// check the ::init_key return code
	if( invalid_key ){
		KLOG_ERR( label << " test vector failed in init_key");
		return true;
	}
	
	// decypher the data
	ssize_t	tmp_plaintxt_len = skey_ciph.decipher( iv, tmp_ciphertxt, tmp_ciphertxt_len
							, tmp_plaintxt, tmp_ciphertxt_len);
	if( tmp_plaintxt_len != (ssize_t)tvect.plaintxt_len ){
		KLOG_ERR(label <<" test vector unciphered doenst produce the expected plain txt length!");
		return true;
	}
	// check if the deciphered data is equal to the original plaintxt
	if( memcmp( tmp_plaintxt, tvect.plaintxt_ptr, tvect.plaintxt_len)){
		KLOG_ERR(label << " test vector unciphered doenst produce the expected plain txt!");
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test aes ecb
 */
nunit_res_t	skey_ciph_testclass_t::aes_ecb(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	skey_ciph_t	skey_ciph	= skey_ciph_t("aes/ecb");
	// first 4 vectors  http://csrc.nist.gov/CryptoToolkit/aes/rijndael/rijndael-vals.zip/ecb_tbl.txt
	neoip_skey_ciph_utest_vector_t	tvect[]	= {
		{ "\x00\x01\x02\x03\x05\x06\x07\x08\x0A\x0B\x0C\x0D\x0F\x10\x11\x12", 16
		  , "", 0
		  , "\x50\x68\x12\xA4\x5F\x08\xC8\x89\xB9\x7F\x59\x80\x03\x8B\x83\x59", 16
		  , "\xD8\xF5\x32\x53\x82\x89\xEF\x7D\x06\xB5\x06\xA4\xFD\x5B\xE9\xC9", 16
		},
		{ "\x14\x15\x16\x17\x19\x1A\x1B\x1C\x1E\x1F\x20\x21\x23\x24\x25\x26", 16
		  , "", 0
		  , "\x5C\x6D\x71\xCA\x30\xDE\x8B\x8B\x00\x54\x99\x84\xD2\xEC\x7D\x4B", 16
		  , "\x59\xAB\x30\xF4\xD4\xEE\x6E\x4F\xF9\x90\x7E\xF6\x5B\x1F\xB6\x8C", 16
		},
		{ "\x28\x29\x2A\x2B\x2D\x2E\x2F\x30\x32\x33\x34\x35\x37\x38\x39\x3A", 16
		  , "", 0
		  , "\x53\xF3\xF4\xC6\x4F\x86\x16\xE4\xE7\xC5\x61\x99\xF4\x8F\x21\xF6", 16
		  , "\xBF\x1E\xD2\xFC\xB2\xAF\x3F\xD4\x14\x43\xB5\x6D\x85\x02\x5C\xB1", 16
		},
		{ "\x3C\x3D\x3E\x3F\x41\x42\x43\x44\x46\x47\x48\x49\x4B\x4C\x4D\x4E", 16
		  , "", 0
		  , "\xA1\xEB\x65\xA3\x48\x71\x65\xFB\x0F\x1C\x27\xFF\x99\x59\xF7\x03", 16
		  , "\x73\x16\x63\x2D\x5C\x32\x23\x3E\xDC\xB0\x78\x05\x60\xEA\xE8\xB2", 16
		},
	};
	// test vectors
	for( size_t i = 0; i < sizeof(tvect)/sizeof(tvect[0]); i ++ ){
		std::string	label = OSTREAMSTR(skey_ciph) + " " + OSTREAMSTR(i+1) + "th";
		bool failed = neoip_skey_ciph_utest_one( skey_ciph, label, tvect[i] );
		if( failed  )	return NUNIT_RES_ERROR;
	}

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test aes cbc
 */
nunit_res_t	skey_ciph_testclass_t::aes_cbc(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	skey_ciph_t	skey_ciph	= skey_ciph_t("aes/cbc");
	neoip_skey_ciph_utest_vector_t	tvect[]= {
		{   "\x06\xa9\x21\x40\x36\xb8\xa1\x5b\x51\x2e\x03\xd5\x34\x12\x00\x06", 16
		  , "\x3d\xaf\xba\x42\x9d\x9e\xb4\x30\xb4\x22\xda\x80\x2c\x9f\xac\x41", 16
		  , "Single block msg", 16
		  , "\xe3\x53\x77\x9c\x10\x79\xae\xb8\x27\x08\x94\x2d\xbe\x77\x18\x1a", 16	
		},
		{   "\xc2\x86\x69\x6d\x88\x7c\x9a\xa0\x61\x1b\xbb\x3e\x20\x25\xa4\x5a", 16
		  , "\x56\x2e\x17\x99\x6d\x09\x3d\x28\xdd\xb3\xba\x69\x5a\x2e\x6f\x58", 16
		  , "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
		  		"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f", 32
		  , "\xd2\x96\xcd\x94\xc2\xcc\xcf\x8a\x3a\x86\x30\x28\xb5\xe1\xdc\x0a"
		  		"\x75\x86\x60\x2d\x25\x3c\xff\xf9\x1b\x82\x66\xbe\xa6\xd6\x1a\xb1", 32
		},
		{   "\x6c\x3e\xa0\x47\x76\x30\xce\x21\xa2\xce\x33\x4a\xa7\x46\xc2\xcd", 16
		  , "\xc7\x82\xdc\x4c\x09\x8c\x66\xcb\xd9\xcd\x27\xd8\x25\x68\x2c\x81", 16
		  , "This is a 48-byte message (exactly 3 AES blocks)", 48
		  , "\xd0\xa0\x2b\x38\x36\x45\x17\x53\xd4\x93\x66\x5d\x33\xf0\xe8\x86"
		  		"\x2d\xea\x54\xcd\xb2\x93\xab\xc7\x50\x69\x39\x27\x67\x72\xf8\xd5"
				"\x02\x1c\x19\x21\x6b\xad\x52\x5c\x85\x79\x69\x5d\x83\xba\x26\x84", 48
		},
		{   "\x56\xe4\x7a\x38\xc5\x59\x89\x74\xbc\x46\x90\x3d\xba\x29\x03\x49", 16
		  , "\x8c\xe8\x2e\xef\xbe\xa0\xda\x3c\x44\x69\x9e\xd7\xdb\x51\xb7\xd9", 16
		  , "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
		  		"\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
		  		"\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
		  		"\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf", 64
		  , "\xc3\x0e\x32\xff\xed\xc0\x77\x4e\x6a\xff\x6a\xf0\x86\x9f\x71\xaa"
		  		"\x0f\x3a\xf0\x7a\x9a\x31\xa9\xc6\x84\xdb\x20\x7e\xb0\xef\x8e\x4e"
		  		"\x35\x90\x7a\xa6\x32\xc3\xff\xdf\x86\x8b\xb7\xb2\x9d\x3d\x46\xad"
		  		"\x83\xce\x9f\x9a\x10\x2e\xe9\x9d\x49\xa5\x3e\x87\xf4\xc3\xda\x55", 64
		},
	};

	// test vectors
	for( size_t i = 0; i < sizeof(tvect)/sizeof(tvect[0]); i ++ ){
		std::string	label = OSTREAMSTR(skey_ciph) + " " + OSTREAMSTR(i+1) + "th";
		bool failed = neoip_skey_ciph_utest_one( skey_ciph, label, tvect[i] );
		if( failed  )	return NUNIT_RES_ERROR;
	}

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test arcfour
 */
nunit_res_t	skey_ciph_testclass_t::arcfour(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	neoip_skey_ciph_utest_vector_t	tvect[]	= {
		// some test vector from http://www.columbia.edu/~ariel/ssleay/rrc4.html
		{ "\x01\x23\x45\x67\x89\xab\xcd\xef", 8
		  , "", 0
		  , "\x01\x23\x45\x67\x89\xab\xcd\xef", 8
		  , "\x75\xb7\x87\x80\x99\xe0\xc5\x96 ", 8
		},
		{ "\x01\x23\x45\x67\x89\xab\xcd\xef", 8
		  , "", 0
		  , "\x00\x00\x00\x00\x00\x00\x00\x00", 8
		  , "\x74\x94\xc2\xe7\x10\x4b\x08\x79", 8
		},
		{ "\x00\x00\x00\x00\x00\x00\x00\x00", 8
		  , "", 0
		  , "\x00\x00\x00\x00\x00\x00\x00\x00", 8
		  , "\xde\x18\x89\x41\xa3\x37\x5d\x3a", 8
		},
#if 0	// TODO currently depending on libgcrypt which has some limitations
	// - despite them saying they are compatible with rc4, they imposes
	//   additionnal limitations... undocumented. they are trying to push
	//   their idea of security... refraining from ranting :)
	// - here the limitation is arcfour keys MUST be > 40-bit
	// - switching out of libgcrypt and co is really a good idea :)
		{ "Key", 3
		  , "", 0
		  , "Plaintext", 9
		  , "\xBB\xF3\x16\xE8\xD9\x40\xAF\x0A\xD3", 9
		},
		{ "Wiki", 4
		  , "", 0
		  , "pedia", 5
		  , "\x10\x21\xBF\x04\x20", 5
		},
#endif
		{ "Secret", 6
		  , "", 0
		  , "Attack at dawn", 14
		  , "\x45\xA0\x1F\x64\x5F\xC3\x5B\x38\x35\x52\x54\x4B\x9B\xF5", 14
		},
	};
	// - another source of rc4 testvector
	//   - http://archives.venona.com/date/1994/09/msg00420.html

	
	// log to debug
	KLOG_DBG("enter");
	
	skey_ciph_t	skey_ciph	= skey_ciph_t("arcfour/stream");

	// test vectors
	for( size_t i = 0; i < sizeof(tvect)/sizeof(tvect[0]); i ++ ){
		std::string	label = OSTREAMSTR(skey_ciph) + " " + OSTREAMSTR(i+1) + "th";
		bool failed = neoip_skey_ciph_utest_one( skey_ciph, label, tvect[i] );
		if( failed  )	return NUNIT_RES_ERROR;
	}

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

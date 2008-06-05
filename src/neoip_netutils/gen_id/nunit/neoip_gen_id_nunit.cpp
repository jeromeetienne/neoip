/*! \file
    \brief unit test for the gen_id_t
*/

/* system include */
/* local include */
#include "neoip_gen_id_nunit.hpp"
#include "neoip_gen_id.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// define the gen_id_utest_t 
// - just put that in a .hpp and you are done 
// - as it is template, no need for a .cpp
NEOIP_GEN_ID_DECLARATION_START(	gen_id_utest_t	, skey_auth_algo_t::SHA1, 20);
NEOIP_GEN_ID_DECLARATION_END(	gen_id_utest_t	, skey_auth_algo_t::SHA1, 20);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the is_null()
 */
nunit_res_t	gen_id_testclass_t::test_is_null(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	gen_id_utest_t	gen_id;
	
	// the gen_id_t constructed by default MUST be null
	NUNIT_ASSERT( gen_id.is_null() );

	// the gen_id_t from a empty string MUST be null
	gen_id	= "";
	NUNIT_ASSERT( gen_id.is_null() );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the serial consistency
 */
nunit_res_t	gen_id_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	gen_id_utest_t	gen_id_toserial = gen_id_utest_t::build_random();
	gen_id_utest_t	gen_id_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << gen_id_toserial;
	serial >> gen_id_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( gen_id_toserial == gen_id_unserial );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the xmlrpc consistency
 */
nunit_res_t	gen_id_testclass_t::xmlrpc_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	gen_id_utest_t	gen_id_toserial = gen_id_utest_t::build_random();
	gen_id_utest_t	gen_id_unserial;	
	xmlrpc_build_t	xmlrpc_build;
	xmlrpc_parse_t	xmlrpc_parse;

	// build a xmlrpc call
	xmlrpc_build	<< 	gen_id_toserial;

	// parse the xmlrpc call which has been just build
	xmlrpc_parse.set_document(xmlrpc_build.to_datum());
	NUNIT_ASSERT( !xmlrpc_parse.is_null() );
	try {
		xmlrpc_parse 	>> 	gen_id_unserial;
	}catch(xml_except_t &e){
		NUNIT_ASSERT( 0 );
	}
	
	// test the serialization consistency
	NUNIT_ASSERT( gen_id_toserial == gen_id_unserial );
	
	// return no error
	return NUNIT_RES_OK;
}



/** \brief test the serial consistency
 */
nunit_res_t	gen_id_testclass_t::canonical_str_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	gen_id_utest_t	gen_id	= gen_id_utest_t::build_random();
	// check the serial consistency
	NUNIT_ASSERT( gen_id_utest_t::from_canonical_string(gen_id.to_canonical_string()) == gen_id );
	// check that it is failing to build a gen_id_utest_t from a non canonical string
	NUNIT_ASSERT( gen_id_utest_t::from_canonical_string("balbla").is_null() );

	// return no error
	return NUNIT_RES_OK;
}
/** \brief test the gen_id_t::derivation_hash()
 * 
 * - test a sha1 test vector from rfc3174 to ensure it does normal SHA1
 */
nunit_res_t	gen_id_testclass_t::derivation_hash(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// test a sha1 test vector from rfc3174 to ensure it does normal SHA1
	// - test1
	NUNIT_ASSERT( gen_id_utest_t("abc") == "0xa9993e364706816aba3e25717850c26c9cd0d89d");
	// - test2
	NUNIT_ASSERT( gen_id_utest_t("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ==
					"0x84983e441c3bd26ebaae4aa1f95129e5e54670f1" );
	// NOTE: the test3 and test4 are not done because rfc3174 reapeat th hash multiple times

	// return no error
	return NUNIT_RES_OK;
}


/** \brief test the gen_id_t::bit_clear_n_highest()
 */
nunit_res_t	gen_id_testclass_t::bit_clear_n_highest(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	gen_id_utest_t	gen_id_utest	= "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
	// test the bit_clear_n_highest function
	NUNIT_ASSERT( gen_id_utest.bit_clear_n_highest(0) == "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
	NUNIT_ASSERT( gen_id_utest.bit_clear_n_highest(2) == "0x3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
	NUNIT_ASSERT( gen_id_utest.bit_clear_n_highest(8) == "0x00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
	NUNIT_ASSERT( gen_id_utest.bit_clear_n_highest(20)== "0x00000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
	NUNIT_ASSERT( gen_id_utest.bit_clear_n_highest(160)=="0x0000000000000000000000000000000000000000" );
	
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the gen_id_t::bit_get_highest_set()
 */
nunit_res_t	gen_id_testclass_t::bit_get_highest_set(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	gen_id_utest_t	gen_id2;
	// test the bit_get_highest_set function
	gen_id2	= "0x0000000000000000000000000000000000000000";
	NUNIT_ASSERT( gen_id2.bit_get_highest_set() == 0   );
	gen_id2	= "0x00000000000000000000000000000000000007FF";
	NUNIT_ASSERT( gen_id2.bit_get_highest_set() == 10  );
	gen_id2	= "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
	NUNIT_ASSERT( gen_id2.bit_get_highest_set() == 159 );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the gen_id_t::test_xor()
 */
nunit_res_t	gen_id_testclass_t::test_xor(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	gen_id_utest_t	gen_id_1	= gen_id_utest_t::build_random();
	gen_id_utest_t	gen_id_2	= gen_id_utest_t::build_random();
	// test the symmetry of the xor
	NUNIT_ASSERT( (gen_id_1 ^ gen_id_2) == (gen_id_2 ^ gen_id_1) );
	// test xor on itself == 0
	NUNIT_ASSERT( (gen_id_1 ^ gen_id_1) == "0x0000000000000000000000000000000000000000");

	// return no error
	return NUNIT_RES_OK;
}


/** \brief test the gen_id_t::test_and()
 */
nunit_res_t	gen_id_testclass_t::test_and(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	gen_id_utest_t	gen_id_1	= "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF1234";	
	gen_id_utest_t	gen_id_2	= "0x0000000000000000000000000000000000000FFF";
	NUNIT_ASSERT( (gen_id_1 & gen_id_2) == "0x0000000000000000000000000000000000000234");

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the gen_id_t::test_rshift()
 */
nunit_res_t	gen_id_testclass_t::test_rshift(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	gen_id_utest_t	gen_id2 = "0xF00000000000000000000000000000000000FF00";
	// test the operator >>
	NUNIT_ASSERT( gen_id2 >> 0 == "0xF00000000000000000000000000000000000FF00" );
	NUNIT_ASSERT( gen_id2 >> 1 == "0x7800000000000000000000000000000000007f80" );
	NUNIT_ASSERT( gen_id2 >> 8 == "0x00f00000000000000000000000000000000000ff" );
	NUNIT_ASSERT( gen_id2 >>156== "0x000000000000000000000000000000000000000f" );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the gen_id_t::comparison_op()
 */
nunit_res_t	gen_id_testclass_t::comparison_op(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	gen_id_utest_t	gen_id_1	= "0x0000000000000000000000000000000000000000";
	gen_id_utest_t	gen_id_2	= "0xF00000000000000000000000000000000000FF00";
	gen_id_utest_t	gen_id_3	= "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
	// some comparison which should be always true
	NUNIT_ASSERT( gen_id_1 < gen_id_2 );
	NUNIT_ASSERT( gen_id_1 < gen_id_3 );
	NUNIT_ASSERT( gen_id_3 > gen_id_2 );
	NUNIT_ASSERT( gen_id_3 > gen_id_1 );
	NUNIT_ASSERT( gen_id_2 == gen_id_2 );
	NUNIT_ASSERT( gen_id_2 <= gen_id_2 );
	NUNIT_ASSERT( gen_id_2 >= gen_id_2 );

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

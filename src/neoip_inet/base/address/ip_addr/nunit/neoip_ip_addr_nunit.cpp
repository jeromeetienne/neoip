/*! \file
    \brief unit test for the ip_addr_t unit test

*/

/* system include */
/* local include */
#include "neoip_ip_addr_nunit.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	ip_addr_testclass_t::convert_from_string(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ip_addr_t		ip_addr	= "10.0.0.2";
	NUNIT_ASSERT( !ip_addr.is_null() );
	// return no error
	return NUNIT_RES_OK;
}


/** \brief test the comparison operator
 */
nunit_res_t	ip_addr_testclass_t::comparison_op(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	NUNIT_ASSERT( ip_addr_t() < ip_addr_t("0.0.0.0") );
	NUNIT_ASSERT( ip_addr_t("10.0.0.1") < ip_addr_t("10.0.0.2") );
	NUNIT_ASSERT( ip_addr_t("12.0.0.1") != ip_addr_t("10.0.0.0") );
	NUNIT_ASSERT( ip_addr_t("1.2.3.4") <= ip_addr_t("2.3.4.5"));
	// return no error
	return NUNIT_RES_OK;
}

/** \brief Test the serialization consistency
 */
nunit_res_t	ip_addr_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ip_addr_t	ip_addr_toserial	= "127.0.0.1";
	ip_addr_t	ip_addr_unserial;
	serial_t	serial;
	// test with ip_addr_t equal to a value
	serial << ip_addr_toserial;
	serial >> ip_addr_unserial;
	NUNIT_ASSERT( ip_addr_toserial == ip_addr_unserial );

	// test with ip_addr_t equal to NULL
	ip_addr_toserial = ip_addr_t();
	serial << ip_addr_toserial;
	serial >> ip_addr_unserial;
	NUNIT_ASSERT( ip_addr_toserial == ip_addr_unserial );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief general test 
 */
nunit_res_t	ip_addr_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	NUNIT_ASSERT( ip_addr_t("10.1.2.3").is_private()	);
	NUNIT_ASSERT( ip_addr_t("192.168.1.2").is_private()	);
	NUNIT_ASSERT( ip_addr_t("127.1.2.3").is_localhost()	);
	NUNIT_ASSERT( ip_addr_t("1.2.3.4").is_public()		);
	NUNIT_ASSERT( ip_addr_t("0.0.0.0").is_any()		);
	NUNIT_ASSERT( ip_addr_t("255.255.255.255").is_broadcast());
	NUNIT_ASSERT( ip_addr_t("1.2.3.4").is_v4()		);
	NUNIT_ASSERT( ip_addr_t("1.2.3.4").get_version() == 4	);
	NUNIT_ASSERT( ip_addr_t("1.2.3.4").get_v4_addr() == 0x01020304);

	// return no error
	return NUNIT_RES_OK;
}



NEOIP_NAMESPACE_END

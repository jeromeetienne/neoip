/*! \file
    \brief unit test for the diffie-hellman unit test

*/

/* system include */
/* local include */
#include "neoip_ip_netaddr_nunit.hpp"
#include "neoip_ip_netaddr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the comparison operator
 */
nunit_res_t	ip_netaddr_testclass_t::comparison_op(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	NUNIT_ASSERT( ip_netaddr_t() < ip_netaddr_t("0.0.0.0/0") );
	NUNIT_ASSERT( ip_netaddr_t("10.0.0.1/8") == ip_netaddr_t("10.0.0.1/8") );
	NUNIT_ASSERT( ip_netaddr_t("10.0.0.1/8") <  ip_netaddr_t("10.0.0.1/16") );
	NUNIT_ASSERT( ip_netaddr_t("10.0.0.1/8") <  ip_netaddr_t("10.0.0.2/8") );
	// return no error
	return NUNIT_RES_OK;
}

/** \brief Test the serialization consistency
 */
nunit_res_t	ip_netaddr_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ip_netaddr_t	ip_netaddr_toserial;
	ip_netaddr_t	ip_netaddr_unserial;
	serial_t	serial;
	// test with ip_netaddr_t equal to a value
	ip_netaddr_toserial = "127.0.0.1/8";
	serial << ip_netaddr_toserial;
	serial >> ip_netaddr_unserial;
	NUNIT_ASSERT( ip_netaddr_toserial == ip_netaddr_unserial );

	// test with ip_netaddr_t equal to NULL
	ip_netaddr_toserial = ip_netaddr_t();
	serial << ip_netaddr_toserial;
	serial >> ip_netaddr_unserial;
	NUNIT_ASSERT( ip_netaddr_toserial == ip_netaddr_unserial );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief general test 
 */
nunit_res_t	ip_netaddr_testclass_t::query_function(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	NUNIT_ASSERT( ip_netaddr_t("10.1.2.3/8").get_base_addr() == "10.1.2.3" );
	NUNIT_ASSERT( ip_netaddr_t("10.1.2.3/8").get_prefix_len() == 8 );
	NUNIT_ASSERT( ip_netaddr_t("10.1.2.3/8").get_netmask() == "255.0.0.0" );
	NUNIT_ASSERT( ip_netaddr_t("10.1.2.3/8").get_any_addr() == "10.0.0.0" );
	NUNIT_ASSERT( ip_netaddr_t("10.1.2.3/8").get_bcast_addr() == "10.255.255.255" );
	NUNIT_ASSERT( ip_netaddr_t("10.1.2.3/8").contain(ip_addr_t("10.0.0.1")) );
	NUNIT_ASSERT( ip_netaddr_t("10.1.2.3/8").contain(ip_addr_t("99.0.0.1")) == false);

	NUNIT_ASSERT( ip_netaddr_t("10.0.0.0/8").get_base_addr() == "10.0.0.0" );
	NUNIT_ASSERT( ip_netaddr_t("10.0.0.0/8").get_first_addr() == "10.0.0.1" );
	NUNIT_ASSERT( ip_netaddr_t("10.0.0.0/8").get_last_addr() == "10.255.255.254" );

	// some test with prefix_len = 32
	NUNIT_ASSERT( ip_netaddr_t("10.0.0.1/32").get_base_addr() == "10.0.0.1" );
	NUNIT_ASSERT( ip_netaddr_t("10.0.0.1/32").get_first_addr() == "10.0.0.1" );
	NUNIT_ASSERT( ip_netaddr_t("10.0.0.1/32").get_last_addr() == "10.0.0.1" );

	// return no error
	return NUNIT_RES_OK;
}



NEOIP_NAMESPACE_END

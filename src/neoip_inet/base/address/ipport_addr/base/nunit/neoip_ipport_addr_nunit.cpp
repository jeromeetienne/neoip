/*! \file
    \brief unit test for the diffie-hellman unit test

*/

/* system include */
/* local include */
#include "neoip_ipport_addr_nunit.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the comparison operator
 */
nunit_res_t	ipport_addr_testclass_t::comparison_op(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	NUNIT_ASSERT( ipport_addr_t() < ipport_addr_t("0.0.0.0:") );
	NUNIT_ASSERT( ipport_addr_t("10.0.0.1:80") == ipport_addr_t("10.0.0.1:80") );
	NUNIT_ASSERT( ipport_addr_t("10.0.0.1:80") <  ipport_addr_t("10.0.0.1:81") );
	NUNIT_ASSERT( ipport_addr_t("10.0.0.1:80") <  ipport_addr_t("10.0.0.2:80") );
	// return no error
	return NUNIT_RES_OK;
}

/** \brief Test the serialization consistency
 */
nunit_res_t	ipport_addr_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ipport_addr_t	ipport_addr_toserial;
	ipport_addr_t	ipport_addr_unserial;
	serial_t	serial;
	// test with ipport_addr_t equal to a value
	ipport_addr_toserial = "127.0.0.1:80";
	serial << ipport_addr_toserial;
	serial >> ipport_addr_unserial;
	NUNIT_ASSERT( ipport_addr_toserial == ipport_addr_unserial );

	// test with ipport_addr_t equal to NULL
	ipport_addr_toserial = ipport_addr_t();
	serial << ipport_addr_toserial;
	serial >> ipport_addr_unserial;
	NUNIT_ASSERT( ipport_addr_toserial == ipport_addr_unserial );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief general test 
 */
nunit_res_t	ipport_addr_testclass_t::query_function(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// test ipaddr:port
	NUNIT_ASSERT( ipport_addr_t("1.2.3.4:500").get_ipaddr() == "1.2.3.4" );
	NUNIT_ASSERT( ipport_addr_t("1.2.3.4:500").get_port() == 500 );
	NUNIT_ASSERT( ipport_addr_t("1.2.3.4:500").is_fully_qualified() );
	NUNIT_ASSERT( ipport_addr_t("1.2.3.4:500").is_null() == false );
	// test :port
	NUNIT_ASSERT( ipport_addr_t(":500").get_ipaddr().is_any() );
	NUNIT_ASSERT( ipport_addr_t(":500").get_port() == 500 );
	NUNIT_ASSERT( ipport_addr_t(":500").is_fully_qualified() == false );
	NUNIT_ASSERT( ipport_addr_t(":500").is_null() == false );
	// test addr
	NUNIT_ASSERT( ipport_addr_t("1.2.3.4").get_ipaddr() == "1.2.3.4" );
	NUNIT_ASSERT( ipport_addr_t("1.2.3.4").get_port() == 0 );
	NUNIT_ASSERT( ipport_addr_t("1.2.3.4").is_fully_qualified() == false );
	NUNIT_ASSERT( ipport_addr_t("1.2.3.4").is_null() == false );
	// test ":"
	NUNIT_ASSERT( ipport_addr_t(":").get_ipaddr().is_any() );
	NUNIT_ASSERT( ipport_addr_t(":").get_port() == 0 );
	NUNIT_ASSERT( ipport_addr_t(":").is_fully_qualified() == false );
	NUNIT_ASSERT( ipport_addr_t(":").is_null() == false );
	// test default ctor
	NUNIT_ASSERT( ipport_addr_t().is_fully_qualified() == false );
	NUNIT_ASSERT( ipport_addr_t().is_null() );
	// return no error
	return NUNIT_RES_OK;
}



NEOIP_NAMESPACE_END

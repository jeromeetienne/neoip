/*! \file
    \brief Definition of the unit test for the \ref socket_addr_t

*/

/* system include */
/* local include */
#include "neoip_socket_addr_nunit.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	socket_addr_testclass_t::test_ctor(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	socket_addr_t	socket_addr;

	// test on a null socket_addr_t
	NUNIT_ASSERT( socket_addr.is_null() );
	NUNIT_ASSERT( socket_addr.get_domain() == socket_domain_t::NONE );

// test the socket_domain_t::UDP
	socket_addr	= "udp://127.0.0.1:41";
	socket_addr	= std::string("udp://127.0.0.1:41");
	NUNIT_ASSERT( !socket_addr.is_null() );
	NUNIT_ASSERT( socket_addr.get_domain() == socket_domain_t::UDP );
	NUNIT_ASSERT( socket_addr.to_string() == "udp://127.0.0.1:41" );

	// return noerror
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	socket_addr_testclass_t::comparison_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	socket_addr_t	addr0	= socket_addr_t();
	socket_addr_t	addr1	= socket_addr_t("udp://127.0.0.1:41");
	socket_addr_t	addr2	= socket_addr_t("udp://127.0.0.2:42");

	// test == and !=
	NUNIT_ASSERT( addr0 == addr0 );
	NUNIT_ASSERT( addr1 == addr1 );
	NUNIT_ASSERT( addr2 == addr2 );
	NUNIT_ASSERT( addr0 != addr1 );
	NUNIT_ASSERT( addr0 != addr2 );
	NUNIT_ASSERT( addr1 != addr2 );
	
	// test greater than and less than
	NUNIT_ASSERT( addr0 < addr1 );
	NUNIT_ASSERT( addr1 > addr0 );
	NUNIT_ASSERT( addr1 < addr2 );
	NUNIT_ASSERT( addr2 > addr1 );

	// return noerror
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	socket_addr_testclass_t::copy_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	socket_addr_t	addr1	= socket_addr_t("udp://127.0.0.1:41");
	socket_addr_t	addr2	= addr1;
	socket_addr_t	addr3(addr1);

	NUNIT_ASSERT( addr1 == addr1 );
	NUNIT_ASSERT( addr2 == addr1 );
	NUNIT_ASSERT( addr3 == addr1 );

	// return noerror
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	socket_addr_testclass_t::serial_with_null(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// test serialization consistency with a null socket_addr_t
	socket_addr_t	socket_addr_toserial;
	socket_addr_t	socket_addr_unserial;
	serial_t	serial;
	// set socket_addr_toserial with a null one
	socket_addr_toserial	= socket_addr_t();
	// do the serial/unserial
	serial << socket_addr_toserial;
	serial >> socket_addr_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( socket_addr_toserial == socket_addr_unserial );

	// return noerror
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	socket_addr_testclass_t::serial_with_domain(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// test serialization consistency with a null socket_addr_t
	socket_addr_t	socket_addr_toserial;
	socket_addr_t	socket_addr_unserial;
	serial_t	serial;
	// set socket_addr_toserial with a null one
	socket_addr_toserial	= socket_addr_t("udp://127.0.0.1:41");
	// do the serial/unserial
	serial << socket_addr_toserial;
	serial >> socket_addr_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( socket_addr_toserial == socket_addr_unserial );

	// return noerror
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END


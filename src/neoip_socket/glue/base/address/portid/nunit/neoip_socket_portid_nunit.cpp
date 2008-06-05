/*! \file
    \brief Definition of the unit test for the \ref socket_portid_t

*/

/* system include */
/* local include */
#include "neoip_socket_portid_nunit.hpp"
#include "neoip_socket_portid.hpp"
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
nunit_res_t	socket_portid_testclass_t::test_ctor(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	socket_portid_t	socket_portid;

	// test on a null socket_portid_t
	NUNIT_ASSERT( socket_portid.is_null() );
	NUNIT_ASSERT( socket_portid.get_domain() == socket_domain_t::NONE );

// test the socket_domain_t::UDP
	socket_portid	= "udp://41";
	socket_portid	= std::string("udp://41");
	NUNIT_ASSERT( !socket_portid.is_null() );
	NUNIT_ASSERT( socket_portid.get_domain() == socket_domain_t::UDP );
	NUNIT_ASSERT( socket_portid.to_string() == "udp://41" );

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
nunit_res_t	socket_portid_testclass_t::comparison_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	socket_portid_t	portid0	= socket_portid_t();
	socket_portid_t	portid1	= socket_portid_t("udp://41");
	socket_portid_t	portid2	= socket_portid_t("udp://42");

	// test == and !=
	NUNIT_ASSERT( portid0 == portid0 );
	NUNIT_ASSERT( portid1 == portid1 );
	NUNIT_ASSERT( portid2 == portid2 );
	NUNIT_ASSERT( portid0 != portid1 );
	NUNIT_ASSERT( portid0 != portid2 );
	NUNIT_ASSERT( portid1 != portid2 );
	
	// test greater than and less than
	NUNIT_ASSERT( portid0 < portid1 );
	NUNIT_ASSERT( portid1 > portid0 );
	NUNIT_ASSERT( portid1 < portid2 );
	NUNIT_ASSERT( portid2 > portid1 );

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
nunit_res_t	socket_portid_testclass_t::copy_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	socket_portid_t	portid1	= socket_portid_t("udp://41");
	socket_portid_t	portid2	= portid1;
	socket_portid_t	portid3(portid1);

	NUNIT_ASSERT( portid1 == portid1 );
	NUNIT_ASSERT( portid2 == portid1 );
	NUNIT_ASSERT( portid3 == portid1 );

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
nunit_res_t	socket_portid_testclass_t::serial_with_null(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// test serialization consistency with a null socket_portid_t
	socket_portid_t	socket_portid_toserial;
	socket_portid_t	socket_portid_unserial;
	serial_t	serial;
	// set socket_portid_toserial with a null one
	socket_portid_toserial	= socket_portid_t();
	// do the serial/unserial
	serial << socket_portid_toserial;
	serial >> socket_portid_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( socket_portid_toserial == socket_portid_unserial );

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
nunit_res_t	socket_portid_testclass_t::serial_with_domain(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// test serialization consistency with a null socket_portid_t
	socket_portid_t	socket_portid_toserial;
	socket_portid_t	socket_portid_unserial;
	serial_t	serial;
	// set socket_portid_toserial with a null one
	socket_portid_toserial	= socket_portid_t("udp://41");
	// do the serial/unserial
	serial << socket_portid_toserial;
	serial >> socket_portid_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( socket_portid_toserial == socket_portid_unserial );

	// return noerror
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END


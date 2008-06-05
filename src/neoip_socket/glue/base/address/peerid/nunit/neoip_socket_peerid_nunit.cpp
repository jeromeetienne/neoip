/*! \file
    \brief Definition of the unit test for the \ref socket_peerid_t

*/

/* system include */
/* local include */
#include "neoip_socket_peerid_nunit.hpp"
#include "neoip_socket_peerid.hpp"
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
nunit_res_t	socket_peerid_testclass_t::test_ctor(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	socket_peerid_t	socket_peerid;

	// test on a null socket_peerid_t
	NUNIT_ASSERT( socket_peerid.is_null() );
	NUNIT_ASSERT( socket_peerid.get_domain() == socket_domain_t::NONE );

// test the socket_domain_t::UDP
	socket_peerid	= "udp://127.0.0.1";
	socket_peerid	= std::string("udp://127.0.0.1");
	NUNIT_ASSERT( !socket_peerid.is_null() );
	NUNIT_ASSERT( socket_peerid.get_domain() == socket_domain_t::UDP );
	NUNIT_ASSERT( socket_peerid.to_string() == "udp://127.0.0.1" );

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
nunit_res_t	socket_peerid_testclass_t::comparison_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	socket_peerid_t	peerid0	= socket_peerid_t();
	socket_peerid_t	peerid1	= socket_peerid_t("udp://127.0.0.1");
	socket_peerid_t	peerid2	= socket_peerid_t("udp://127.0.0.2");

	// test == and !=
	NUNIT_ASSERT( peerid0 == peerid0 );
	NUNIT_ASSERT( peerid1 == peerid1 );
	NUNIT_ASSERT( peerid2 == peerid2 );
	NUNIT_ASSERT( peerid0 != peerid1 );
	NUNIT_ASSERT( peerid0 != peerid2 );
	NUNIT_ASSERT( peerid1 != peerid2 );
	
	// test greater than and less than
	NUNIT_ASSERT( peerid0 < peerid1 );
	NUNIT_ASSERT( peerid1 > peerid0 );
	NUNIT_ASSERT( peerid1 < peerid2 );
	NUNIT_ASSERT( peerid2 > peerid1 );

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
nunit_res_t	socket_peerid_testclass_t::copy_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	socket_peerid_t	peerid1	= socket_peerid_t("udp://127.0.0.1");
	socket_peerid_t	peerid2	= peerid1;
	socket_peerid_t	peerid3(peerid1);

	NUNIT_ASSERT( peerid1 == peerid1 );
	NUNIT_ASSERT( peerid2 == peerid1 );
	NUNIT_ASSERT( peerid3 == peerid1 );

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
nunit_res_t	socket_peerid_testclass_t::serial_with_null(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// test serialization consistency with a null socket_peerid_t
	socket_peerid_t	socket_peerid_toserial;
	socket_peerid_t	socket_peerid_unserial;
	serial_t	serial;
	// set socket_peerid_toserial with a null one
	socket_peerid_toserial	= socket_peerid_t();
	// do the serial/unserial
	serial << socket_peerid_toserial;
	serial >> socket_peerid_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( socket_peerid_toserial == socket_peerid_unserial );

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
nunit_res_t	socket_peerid_testclass_t::serial_with_domain(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// test serialization consistency with a null socket_peerid_t
	socket_peerid_t	socket_peerid_toserial;
	socket_peerid_t	socket_peerid_unserial;
	serial_t	serial;
	// set socket_peerid_toserial with a null one
	socket_peerid_toserial	= socket_peerid_t("udp://127.0.0.1");
	// do the serial/unserial
	serial << socket_peerid_toserial;
	serial >> socket_peerid_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( socket_peerid_toserial == socket_peerid_unserial );

	// return noerror
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END


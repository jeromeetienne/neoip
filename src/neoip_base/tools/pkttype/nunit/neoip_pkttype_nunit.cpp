/*! \file
    \brief Definition of the unit test for the \ref pkttype_t
*/

/* system include */
/* local include */
#include "neoip_pkttype_nunit.hpp"
#include "neoip_pkttype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// declaration of a pkttype for the nunit
NEOIP_PKTTYPE_DECLARATION_START(nunit_pkttype_t);
NEOIP_PKTTYPE_DECLARATION_ITEM(nunit_pkttype_t, PKT_REQUEST);
NEOIP_PKTTYPE_DECLARATION_ITEM(nunit_pkttype_t, PKT_REPLY);
NEOIP_PKTTYPE_DECLARATION_END(nunit_pkttype_t);
 
// definition of a pkttype for the nunit
NEOIP_PKTTYPE_DEFINITION_START(nunit_pkttype_t);
NEOIP_PKTTYPE_DEFINITION_ITEM(nunit_pkttype_t, PKT_REQUEST);
NEOIP_PKTTYPE_DEFINITION_ITEM(nunit_pkttype_t, PKT_REPLY);
NEOIP_PKTTYPE_DEFINITION_END(nunit_pkttype_t);

// declaration of a pkttype for the nunit
NEOIP_PKTTYPE_DECLARATION_START(nunit_pkttype2_t);
NEOIP_PKTTYPE_DECLARATION_ITEM(nunit_pkttype2_t, PKT_PROBE);
NEOIP_PKTTYPE_DECLARATION_ITEM(nunit_pkttype2_t, PKT_ACK);
NEOIP_PKTTYPE_DECLARATION_END(nunit_pkttype2_t);
 
// definition of a pkttype for the nunit
NEOIP_PKTTYPE_DEFINITION_START(nunit_pkttype2_t);
NEOIP_PKTTYPE_DEFINITION_ITEM(nunit_pkttype2_t, PKT_PROBE);
NEOIP_PKTTYPE_DEFINITION_ITEM(nunit_pkttype2_t, PKT_ACK);
NEOIP_PKTTYPE_DEFINITION_END(nunit_pkttype2_t);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            testclass
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a pkttype_t
 */
nunit_res_t	pkttype_testclass_t::test_null(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// set variable
	pkttype_profile_t	profile(10, 2, pkttype_profile_t::UINT32);
	nunit_pkttype_t		pkttype(profile);
	// test the .is_null()
	NUNIT_ASSERT( pkttype.is_null() == true );
	pkttype	= pkttype.PKT_REPLY();
	NUNIT_ASSERT( pkttype.is_null() != true );
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            testclass
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a pkttype_t
 */
nunit_res_t	pkttype_testclass_t::comparison(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// set variable
	pkttype_profile_t	profile(10, 2, pkttype_profile_t::UINT32);
	nunit_pkttype_t		pkttype = nunit_pkttype_t(profile).PKT_REQUEST();
	// do some comparison
	NUNIT_ASSERT( pkttype == pkttype.PKT_REQUEST() );
	NUNIT_ASSERT( pkttype != pkttype.PKT_REPLY() );
	NUNIT_ASSERT( pkttype <  pkttype.PKT_REPLY() );
	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            testclass
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a pkttype_t
 */
nunit_res_t	pkttype_testclass_t::serial_offset(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            testclass
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a pkttype_t
 */
nunit_res_t	pkttype_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	pkttype_profile_t	profile(10, 16, pkttype_profile_t::UINT16);
	nunit_pkttype_t		pkttype_toserial = nunit_pkttype_t(profile).PKT_REQUEST();
	nunit_pkttype_t		pkttype_unserial(profile);
	serial_t		serial;
	// log to debug
	KLOG_DBG("enter");
	// do the serial/unserial
	serial << pkttype_toserial;
	serial >> pkttype_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( pkttype_toserial == pkttype_unserial );
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END


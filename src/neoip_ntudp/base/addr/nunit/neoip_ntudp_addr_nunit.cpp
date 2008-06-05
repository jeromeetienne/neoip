/*! \file
    \brief Definition of the unit test for the \ref ntudp_addr_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_addr_nunit.hpp"
#include "neoip_ntudp_addr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief test serial_t consistency of ntudp_addr_t
 */
nunit_res_t	ntudp_addr_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ntudp_addr_t	ntudp_addr_toserial;
	ntudp_addr_t	ntudp_addr_unserial;
	serial_t	serial;
	// set ntudp_addr_toserial
	ntudp_addr_toserial	= ntudp_addr_t(ntudp_peerid_t::build_random()
						, ntudp_portid_t::build_random());
	// do the serial/unserial
	serial << ntudp_addr_toserial;
	serial >> ntudp_addr_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( ntudp_addr_toserial == ntudp_addr_unserial );

	// return no error
	return NUNIT_RES_OK;
}


/** \brief test the ntudp_addr_t contructor from a string
 */
nunit_res_t	ntudp_addr_testclass_t::ctor_from_string(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ntudp_peerid_t	peerid	= "alice peer";
	ntudp_portid_t	portid	= "alice port";
	ntudp_addr_t	addr1	= "alice peer:alice port";
	ntudp_addr_t	addr2(peerid, portid);

	NUNIT_ASSERT( addr1.peerid() == peerid );
	NUNIT_ASSERT( addr1.portid() == portid );
	NUNIT_ASSERT( addr1 == addr2 );

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END


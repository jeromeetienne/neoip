/*! \file
    \brief Definition of the unit test for the \ref ntudp_rdvpt_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_rdvpt_nunit.hpp"
#include "neoip_ntudp_rdvpt.hpp"
#include "neoip_ntudp_rdvpt_arr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief test serial_t consistency of ntudp_rdvpt_t
 */
nunit_res_t	ntudp_rdvpt_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ntudp_rdvpt_t	ntudp_rdvpt_toserial;
	ntudp_rdvpt_t	ntudp_rdvpt_unserial;
	serial_t	serial;
	// set ntudp_rdvpt_toserial
	ntudp_rdvpt_toserial	= ntudp_rdvpt_t("1.2.3.4:5", ntudp_rdvpt_t::RELAY);
	// do the serial/unserial
	serial << ntudp_rdvpt_toserial;
	serial >> ntudp_rdvpt_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( ntudp_rdvpt_toserial == ntudp_rdvpt_unserial );

	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END


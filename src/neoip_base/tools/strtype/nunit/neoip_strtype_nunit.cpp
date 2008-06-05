/*! \file
    \brief Definition of the unit test for the \ref strtype_t
*/

/* system include */
/* local include */
#include "neoip_strtype_nunit.hpp"
#include "neoip_strtype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// test of forward declaration - impossible to do due to type issues... 
class strtype_testo_t;

// the declaration
NEOIP_STRTYPE_DECLARATION_START(strtype_testo_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(strtype_testo_t	, BLABLA)
NEOIP_STRTYPE_DECLARATION_ITEM(strtype_testo_t	, BLIBLI)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(strtype_testo_t)
NEOIP_STRTYPE_DECLARATION_END(strtype_testo_t	, uint8_t)

// the definition
NEOIP_STRTYPE_DEFINITION_START(strtype_testo_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(strtype_testo_t	, BLABLA	, "text for BLABLA" )
NEOIP_STRTYPE_DEFINITION_ITEM(strtype_testo_t	, BLIBLI	, "text for BLIBLI" )
NEOIP_STRTYPE_DEFINITION_END(strtype_testo_t)

/** \brief function to test a strtype_t
 */
nunit_res_t	strtype_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// declare the object
	strtype_testo_t	strtype;
	// to an assignation
	strtype	= strtype_testo_t::BLABLA;
	// check the to_string()
	NUNIT_ASSERT(strtype.to_string() == "text for BLABLA");
	// test the comparision operator
	NUNIT_ASSERT(strtype == strtype_testo_t::BLABLA);
	
	// test the 'array' api
	NUNIT_ASSERT( strtype_testo_t::size() == 2 );
	NUNIT_ASSERT( strtype_testo_t::at(0) == strtype_testo_t::BLABLA );
	NUNIT_ASSERT( strtype_testo_t::at(1) == strtype_testo_t::BLIBLI );

	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END


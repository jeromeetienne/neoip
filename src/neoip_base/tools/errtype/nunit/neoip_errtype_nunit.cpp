/*! \file
    \brief Definition of the unit test for the \ref errtype_t
*/

/* system include */
/* local include */
#include "neoip_errtype_nunit.hpp"
#include "neoip_errtype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// test of forward declaration
class errtype_testo_err_t;

// the declaration
NEOIP_ERRTYPE_DECLARATION_START(errtype_testo_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(errtype_testo_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(errtype_testo_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(errtype_testo_err_t)
NEOIP_ERRTYPE_DECLARATION_END(errtype_testo_err_t)

// the definition
NEOIP_ERRTYPE_DEFINITION_START(errtype_testo_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(errtype_testo_err_t	, OK			, "OK" )
NEOIP_ERRTYPE_DEFINITION_ITEM(errtype_testo_err_t	, ERROR			, "ERROR" )
NEOIP_ERRTYPE_DEFINITION_END(errtype_testo_err_t)

/** \brief function to test a errtype_t
 */
nunit_res_t	errtype_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// declare the object
	errtype_testo_err_t	err;
	// to an assignation
	err	= errtype_testo_err_t::OK;
	// check the to_string()
	NUNIT_ASSERT(err.to_string() == "OK");
	// test the reason get
	NUNIT_ASSERT(err.get_reason() == std::string());	
	// test the comparision operator
	NUNIT_ASSERT(err == errtype_testo_err_t::OK);
	// test the assignement with a reason
	err	= errtype_testo_err_t(errtype_testo_err_t::ERROR, "Something real bad");
	// test the comparision operator
	NUNIT_ASSERT(err == errtype_testo_err_t::ERROR);
	// test the reason get
	NUNIT_ASSERT(err.get_reason() == "Something real bad");
	// assigne the error with a reason - to check that the copy operator do handle the reason copy
	errtype_testo_err_t	err2	= err;
	// copy the error with a reason - to check that the copy operator do handle the reason copy
	errtype_testo_err_t	err3(err);
	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END


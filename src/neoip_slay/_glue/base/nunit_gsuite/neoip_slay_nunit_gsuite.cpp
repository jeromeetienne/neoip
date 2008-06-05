/*! \file
    \brief Definition of the nunit_suite_t

*/

/* system include */
/* local include */
#include "neoip_slay_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of include for each testclass
#include "neoip_slay_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit_gsuite_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Implement a nunit_gsuite_fct_t for the kad layer
 */
static void slay_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	slay_suite	= nipmem_new nunit_suite_t("slay");	
	// log to debug
	KLOG_DBG("enter");

/********************* slay_t	***************************************/
	// init the testclass for the slay_t
	nunit_testclass_t<slay_testclass_t> *	slay_testclass;
	slay_testclass	= nipmem_new nunit_testclass_t<slay_testclass_t>("inmemory", nipmem_new slay_testclass_t());
	// add some test functions
	slay_testclass->append("standalone_tls"		, &slay_testclass_t::standalone_tls);
	slay_testclass->append("standalone_btjamstd"	, &slay_testclass_t::standalone_btjamstd);
	slay_testclass->append("standalone_btjamrc4"	, &slay_testclass_t::standalone_btjamrc4);
	// add the testclass to the nunit_suite
	slay_suite->append(slay_testclass);


	// add the slay_suite to the nunit_suite
	nunit_suite.append(slay_suite);	
}

// definition of the nunit_gsuite_t for kad
NUNIT_GSUITE_DEFINITION(slay_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(slay_nunit_gsuite, 0, slay_gsuite_fct);

NEOIP_NAMESPACE_END





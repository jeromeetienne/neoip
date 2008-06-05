/*! \file
    \brief Definition of the nunit_suite_t

*/

/* system include */
/* local include */
#include "neoip_eloop_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// include for the nunit testclass
#include "neoip_timeout_nunit.hpp"
#include "neoip_idleop_nunit.hpp"
#include "neoip_agethostbyname_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void eloop_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	eloop_suite	= nipmem_new nunit_suite_t("eloop");		
	// log to debug
	KLOG_DBG("enter");
	
/********************* timeout_t	***************************************/
	// init the testclass for the timeout_t
	nunit_testclass_t<timeout_testclass_t> *	timeout_testclass;
	timeout_testclass	= nipmem_new nunit_testclass_t<timeout_testclass_t>("timeout"
							, nipmem_new timeout_testclass_t());
	// add some test functions
	timeout_testclass->append("expiration"		, &timeout_testclass_t::expiration);
	timeout_testclass->append("special_delay"	, &timeout_testclass_t::special_delay);
	// add the testclass to the nunit_suite
	eloop_suite->append(timeout_testclass);

#ifdef USE_ELOOP_GLIB
/********************* idleop_t	***************************************/
	// init the testclass for the idleop_t
	nunit_testclass_t<idleop_testclass_t> *	idleop_testclass;
	idleop_testclass	= nipmem_new nunit_testclass_t<idleop_testclass_t>("idleop"
							, nipmem_new idleop_testclass_t());
	// add some test functions
	idleop_testclass->append("general"	, &idleop_testclass_t::general);
	// add the testclass to the nunit_suite
	eloop_suite->append(idleop_testclass);
	
/********************* agethostbyname_t	***************************************/
	// init the testclass for the agethostbyname_t
	nunit_testclass_t<agethostbyname_testclass_t> *	agethostbyname_testclass;
	agethostbyname_testclass	= nipmem_new nunit_testclass_t<agethostbyname_testclass_t>("agethostbyname", nipmem_new agethostbyname_testclass_t());
	// add some test functions
	agethostbyname_testclass->append("query_localhost"	, &agethostbyname_testclass_t::query_localhost);
	// add the testclass to the nunit_suite
	eloop_suite->append(agethostbyname_testclass);
#endif

	// add the eloop_suite to the nunit_suite
	nunit_suite.append(eloop_suite);
}

// definition of the nunit_gsuite_t for eloop
NUNIT_GSUITE_DEFINITION(eloop_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(eloop_nunit_gsuite, 0, eloop_gsuite_fct);

NEOIP_NAMESPACE_END





/*! \file
    \brief Definition of the nunit_suite_t

*/

/* system include */
/* local include */
#include "neoip_libsess_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// include for the nunit testclass
#include "neoip_clineopt_nunit.hpp"
#include "neoip_asyncexe_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void libsess_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	libsess_suite	= nipmem_new nunit_suite_t("libsess");		
	// log to debug
	KLOG_DBG("enter");

/********************* clineopt_t	***************************************/
	// init the testclass for the clineopt_t
	nunit_testclass_t<clineopt_testclass_t> *	clineopt_testclass;
	clineopt_testclass	= nipmem_new nunit_testclass_t<clineopt_testclass_t>("clineopt"
							, nipmem_new clineopt_testclass_t());
	// add some test functions
	clineopt_testclass->append("general"		, &clineopt_testclass_t::general);
	// add the testclass to the nunit_suite
	libsess_suite->append(clineopt_testclass);

#ifndef	_WIN32	// asyncexe_t is not available under _WIN32
/********************* asyncexe_t	***************************************/
	// init the testclass for the asyncexe_t
	nunit_testclass_t<asyncexe_testclass_t> *	asyncexe_testclass;
	asyncexe_testclass	= nipmem_new nunit_testclass_t<asyncexe_testclass_t>("asyncexe"
							, nipmem_new asyncexe_testclass_t());
	// add some test functions
	asyncexe_testclass->append("general"		, &asyncexe_testclass_t::general);
	// add the testclass to the nunit_suite
	libsess_suite->append(asyncexe_testclass);
#endif

	// add the libsess_suite to the nunit_suite
	nunit_suite.append(libsess_suite);
}

// definition of the nunit_gsuite_t for base
NUNIT_GSUITE_DEFINITION(libsess_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(libsess_nunit_gsuite, 0, libsess_gsuite_fct);

NEOIP_NAMESPACE_END





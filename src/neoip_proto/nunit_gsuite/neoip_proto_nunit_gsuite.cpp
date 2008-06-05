/*! \file
    \brief Definition of the nunit_suite_t

*/

/* system include */
/* local include */
#include "neoip_proto_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_pktfrag_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit_gsuite_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Implement a nunit_gsuite_fct_t for the kad layer
 */
static void proto_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	proto_suite	= nipmem_new nunit_suite_t("proto");	
	// log to debug
	KLOG_DBG("enter");

/********************* pktfrag_t	***************************************/
	// init the testclass for the pktfrag_t
	nunit_testclass_t<pktfrag_testclass_t> *	pktfrag_testclass;
	pktfrag_testclass	= nipmem_new nunit_testclass_t<pktfrag_testclass_t>("pktfrag_t"
							, nipmem_new pktfrag_testclass_t());
	// add some test functions
	pktfrag_testclass->append("fragreasm", &pktfrag_testclass_t::fragreasm);
	// add the testclass to the nunit_suite
	proto_suite->append(pktfrag_testclass);


	// add the proto_suite to the nunit_suite
	nunit_suite.append(proto_suite);	
}

// definition of the nunit_gsuite_t for kad
NUNIT_GSUITE_DEFINITION(proto_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(proto_nunit_gsuite, 0, proto_gsuite_fct);

NEOIP_NAMESPACE_END





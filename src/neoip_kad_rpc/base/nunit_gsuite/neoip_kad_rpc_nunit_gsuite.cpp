/*! \file
    \brief Definition of the nunit_gsuite_t for the kad_rpc_t layer

*/

/* system include */
/* local include */
#include "neoip_kad_rpc_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_kad_rpcresp_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void kad_rpc_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	kad_rpc_suite	= nipmem_new nunit_suite_t("kad_rpc");	
	// log to debug
	KLOG_DBG("enter");

#if 0	// TODO to write a real nunit - currently it is just launch a kad_rpcresp_t and up to 
	// a human to write xmlrpc to it
/********************* kad_rpcresp_t	***************************************/
	// init the testclass for the kad_rpcresp_t
	nunit_testclass_t<kad_rpcresp_testclass_t> *	kad_rpcresp_testclass;
	kad_rpcresp_testclass	= nipmem_new nunit_testclass_t<kad_rpcresp_testclass_t>("rpcresp"
							, nipmem_new kad_rpcresp_testclass_t());
	// add some test functions
	kad_rpcresp_testclass->append("general"		, &kad_rpcresp_testclass_t::general);
	// add the testclass to the nunit_suite
	kad_rpc_suite->append(kad_rpcresp_testclass);
#endif

	// add the kad_rpc_suite to the nunit_suite
	nunit_suite.append(kad_rpc_suite);
}

// definition of the nunit_gsuite_t for kad_rpctracker
NUNIT_GSUITE_DEFINITION(kad_rpc_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(kad_rpc_nunit_gsuite, 0, kad_rpc_gsuite_fct);

NEOIP_NAMESPACE_END





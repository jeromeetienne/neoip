/*! \file
    \brief Definition of the nunit_gsuite_t for the xmlrpc_net_tracker_t layer

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_net_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_xmlrpc_resp0_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void xmlrpc_net_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	xmlrpc_net_suite	= nipmem_new nunit_suite_t("xmlrpc_net");	
	// log to debug
	KLOG_DBG("enter");

/********************* xmlrpc_resp0_t	***************************************/
	// init the testclass for the xmlrpc_resp0_t
	nunit_testclass_t<xmlrpc_resp0_testclass_t> *	xmlrpc_resp0_testclass;
	xmlrpc_resp0_testclass	= nipmem_new nunit_testclass_t<xmlrpc_resp0_testclass_t>("resp0"
							, nipmem_new xmlrpc_resp0_testclass_t());
	// add some test functions
	xmlrpc_resp0_testclass->append("general"	, &xmlrpc_resp0_testclass_t::general);
	// add the testclass to the nunit_suite
	xmlrpc_net_suite->append(xmlrpc_resp0_testclass);

	// add the xmlrpc_net_suite to the nunit_suite
	nunit_suite.append(xmlrpc_net_suite);
}

// definition of the nunit_gsuite_t for xmlrpc_net_tracker
NUNIT_GSUITE_DEFINITION(xmlrpc_net_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(xmlrpc_net_nunit_gsuite, 0, xmlrpc_net_gsuite_fct);

NEOIP_NAMESPACE_END





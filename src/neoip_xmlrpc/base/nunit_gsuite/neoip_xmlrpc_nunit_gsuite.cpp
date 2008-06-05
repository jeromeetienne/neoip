/*! \file
    \brief Definition of the nunit_gsuite_t for the xmlrpc_tracker_t layer

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_xmlrpc_build_nunit.hpp"
#include "neoip_xmlrpc_parse_nunit.hpp"
#include "neoip_xmlrpc_consistency_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void xmlrpc_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	xmlrpc_suite	= nipmem_new nunit_suite_t("xmlrpc");	
	// log to debug
	KLOG_DBG("enter");

/********************* xmlrpc_build_t	***************************************/
	// init the testclass for the xmlrpc_build_t
	nunit_testclass_t<xmlrpc_build_testclass_t> *	xmlrpc_build_testclass;
	xmlrpc_build_testclass	= nipmem_new nunit_testclass_t<xmlrpc_build_testclass_t>("build"
							, nipmem_new xmlrpc_build_testclass_t());
	// add some test functions
	xmlrpc_build_testclass->append("example_from_spec"	, &xmlrpc_build_testclass_t::example_from_spec);
	// add the testclass to the nunit_suite
	xmlrpc_suite->append(xmlrpc_build_testclass);
	
/********************* xmlrpc_parse_t	***************************************/
	// init the testclass for the xmlrpc_parse_t
	nunit_testclass_t<xmlrpc_parse_testclass_t> *	xmlrpc_parse_testclass;
	xmlrpc_parse_testclass	= nipmem_new nunit_testclass_t<xmlrpc_parse_testclass_t>("parse"
							, nipmem_new xmlrpc_parse_testclass_t());
	// add some test functions
	xmlrpc_parse_testclass->append("example_from_spec"	, &xmlrpc_parse_testclass_t::example_from_spec);
	xmlrpc_parse_testclass->append("to_json"		, &xmlrpc_parse_testclass_t::to_json);
	// add the testclass to the nunit_suite
	xmlrpc_suite->append(xmlrpc_parse_testclass);

/********************* xmlrpc_consistency_t	***************************************/
	// init the testclass for the xmlrpc_consistency_t
	nunit_testclass_t<xmlrpc_consistency_testclass_t> *	xmlrpc_consistency_testclass;
	xmlrpc_consistency_testclass	= nipmem_new nunit_testclass_t<xmlrpc_consistency_testclass_t>("consistency"
							, nipmem_new xmlrpc_consistency_testclass_t());
	// add some test functions
	xmlrpc_consistency_testclass->append("call_example_from_spec"	, &xmlrpc_consistency_testclass_t::call_example_from_spec);
	xmlrpc_consistency_testclass->append("resp_example_from_spec"	, &xmlrpc_consistency_testclass_t::resp_example_from_spec);
	xmlrpc_consistency_testclass->append("fault_example_from_spec"	, &xmlrpc_consistency_testclass_t::fault_example_from_spec);
	xmlrpc_consistency_testclass->append("call_with_empty_array"	, &xmlrpc_consistency_testclass_t::call_with_empty_array);
	xmlrpc_consistency_testclass->append("call_with_no_param"	, &xmlrpc_consistency_testclass_t::call_with_no_param);
	xmlrpc_consistency_testclass->append("resp_with_empty_struct"	, &xmlrpc_consistency_testclass_t::resp_with_empty_struct);
	xmlrpc_consistency_testclass->append("resp_with_complex_struct", &xmlrpc_consistency_testclass_t::resp_with_complex_struct);
	xmlrpc_consistency_testclass->append("invalid_xml"		, &xmlrpc_consistency_testclass_t::invalid_xml);
	// add the testclass to the nunit_suite
	xmlrpc_suite->append(xmlrpc_consistency_testclass);

	// add the xmlrpc_suite to the nunit_suite
	nunit_suite.append(xmlrpc_suite);
}

// definition of the nunit_gsuite_t for xmlrpc_tracker
NUNIT_GSUITE_DEFINITION(xmlrpc_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(xmlrpc_nunit_gsuite, 0, xmlrpc_gsuite_fct);

NEOIP_NAMESPACE_END





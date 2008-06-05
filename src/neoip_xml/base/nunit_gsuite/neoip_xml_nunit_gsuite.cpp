/*! \file
    \brief Definition of the nunit_gsuite_t for the xml_tracker_t layer

*/

/* system include */
/* local include */
#include "neoip_xml_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_xml_build_nunit.hpp"
#include "neoip_xml_parse_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for libneoip_xml
 */
static void xml_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	xml_suite	= nipmem_new nunit_suite_t("xml");	
	// log to debug
	KLOG_DBG("enter");

/********************* xml_build_t	***************************************/
	// init the testclass for the xml_build_t
	nunit_testclass_t<xml_build_testclass_t> *	xml_build_testclass;
	xml_build_testclass	= nipmem_new nunit_testclass_t<xml_build_testclass_t>("build"
							, nipmem_new xml_build_testclass_t());
	// add some test functions
	xml_build_testclass->append("general"	, &xml_build_testclass_t::general);
	// add the testclass to the nunit_suite
	xml_suite->append(xml_build_testclass);

/********************* xml_parse_t	***************************************/
	// init the testclass for the xml_parse_t
	nunit_testclass_t<xml_parse_testclass_t> *	xml_parse_testclass;
	xml_parse_testclass	= nipmem_new nunit_testclass_t<xml_parse_testclass_t>("parse"
							, nipmem_new xml_parse_testclass_t());
	// add some test functions
	xml_parse_testclass->append("general"	, &xml_parse_testclass_t::general);
	// add the testclass to the nunit_suite
	xml_suite->append(xml_parse_testclass);

	// add the xml_suite to the nunit_suite
	nunit_suite.append(xml_suite);
}

// definition of the nunit_gsuite_t for xml_tracker
NUNIT_GSUITE_DEFINITION(xml_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(xml_nunit_gsuite, 0, xml_gsuite_fct);

NEOIP_NAMESPACE_END





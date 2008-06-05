/*! \file
    \brief Definition of the nunit_gsuite_t for the http_t layer

*/

/* system include */
/* local include */
#include "neoip_http_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of the include for each nunit
#include "neoip_http_uri_nunit.hpp"
#include "neoip_http_reqhd_nunit.hpp"
#include "neoip_http_rephd_nunit.hpp"
#include "neoip_http_listener_nunit.hpp"
#include "neoip_http_client_nunit.hpp"
#include "neoip_http_sclient_nunit.hpp"
#include "neoip_http_nested_uri_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void http_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	http_suite	= nipmem_new nunit_suite_t("http");		
	// log to debug
	KLOG_DBG("enter");

/********************* http_uri_t	***************************************/
	// init the testclass for the http_uri_t
	nunit_testclass_t<http_uri_testclass_t> *	http_uri_testclass;
	http_uri_testclass	= nipmem_new nunit_testclass_t<http_uri_testclass_t>("uri", nipmem_new http_uri_testclass_t());
	// add some test functions
	http_uri_testclass->append("general"		, &http_uri_testclass_t::general);
	http_uri_testclass->append("serial_consistency"	, &http_uri_testclass_t::serial_consistency);
	http_uri_testclass->append("scramble"		, &http_uri_testclass_t::scramble);
	// add the testclass to the nunit_suite
	http_suite->append(http_uri_testclass);

/********************* http_reqhd_t	***************************************/
	// init the testclass for the http_reqhd_t
	nunit_testclass_t<http_reqhd_testclass_t> *	http_reqhd_testclass;
	http_reqhd_testclass	= nipmem_new nunit_testclass_t<http_reqhd_testclass_t>("reqhd", nipmem_new http_reqhd_testclass_t());
	// add some test functions
	http_reqhd_testclass->append("serial_consistency"	, &http_reqhd_testclass_t::serial_consistency);
	http_reqhd_testclass->append("header_range"		, &http_reqhd_testclass_t::header_range);
	// add the testclass to the nunit_suite
	http_suite->append(http_reqhd_testclass);

/********************* http_rephd_t	***************************************/
	// init the testclass for the http_rephd_t
	nunit_testclass_t<http_rephd_testclass_t> *	http_rephd_testclass;
	http_rephd_testclass	= nipmem_new nunit_testclass_t<http_rephd_testclass_t>("rephd", nipmem_new http_rephd_testclass_t());
	// add some test functions
	http_rephd_testclass->append("serial_consistency"	, &http_rephd_testclass_t::serial_consistency);
	http_rephd_testclass->append("header_content_length"	, &http_rephd_testclass_t::header_content_length);
	http_rephd_testclass->append("header_content_range"	, &http_rephd_testclass_t::header_content_range);
	// add the testclass to the nunit_suite
	http_suite->append(http_rephd_testclass);

#if 0	// TODO to reenable when it is a nunit - currently it is only a never ending testbed
/********************* http_listener_t	***************************************/
	// init the testclass for the http_listener_t
	nunit_testclass_t<http_listener_testclass_t> *	http_listener_testclass;
	http_listener_testclass	= nipmem_new nunit_testclass_t<http_listener_testclass_t>("listener", nipmem_new http_listener_testclass_t());
	// add some test functions
	http_listener_testclass->append("general"	, &http_listener_testclass_t::general);
	// add the testclass to the nunit_suite
	http_suite->append(http_listener_testclass);
#endif

/********************* http_client_t	***************************************/
	// init the testclass for the http_client_t
	nunit_testclass_t<http_client_testclass_t> *	http_client_testclass;
	http_client_testclass	= nipmem_new nunit_testclass_t<http_client_testclass_t>("client", nipmem_new http_client_testclass_t());
	// add some test functions
	http_client_testclass->append("one_cnx"	, &http_client_testclass_t::one_cnx);
	// add the testclass to the nunit_suite
	http_suite->append(http_client_testclass);

/********************* http_sclient_t	***************************************/
	// init the testclass for the http_sclient_t
	nunit_testclass_t<http_sclient_testclass_t> *	http_sclient_testclass;
	http_sclient_testclass	= nipmem_new nunit_testclass_t<http_sclient_testclass_t>("sclient", nipmem_new http_sclient_testclass_t());
	// add some test functions
	http_sclient_testclass->append("one_cnx"	, &http_sclient_testclass_t::one_cnx);
	// add the testclass to the nunit_suite
	http_suite->append(http_sclient_testclass);

/********************* http_nested_uri_t	***************************************/
	// init the testclass for the http_nested_uri_t
	nunit_testclass_t<http_nested_uri_testclass_t> *http_nested_uri_testclass;
	http_nested_uri_testclass	= nipmem_new nunit_testclass_t<http_nested_uri_testclass_t>("nested_uri", nipmem_new http_nested_uri_testclass_t());
	// add some test functions
	http_nested_uri_testclass->append("is_valid_outter"	, &http_nested_uri_testclass_t::is_valid_outter);
	http_nested_uri_testclass->append("nested_consistency1"	, &http_nested_uri_testclass_t::nested_consistency1);
	http_nested_uri_testclass->append("nested_consistency2"	, &http_nested_uri_testclass_t::nested_consistency2);
	// add the testclass to the nunit_suite
	http_suite->append(http_nested_uri_testclass);

	// add the http_suite to the nunit_suite
	nunit_suite.append(http_suite);	
}

// definition of the nunit_gsuite_t for http
NUNIT_GSUITE_DEFINITION(http_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(http_nunit_gsuite, 0, http_gsuite_fct);

NEOIP_NAMESPACE_END





/*! \file
    \brief Definition of the nunit_gsuite_t for the router_t layer

*/

/* system include */
/* local include */
#include "neoip_router_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_router_lident_nunit.hpp"
#include "neoip_router_rident_nunit.hpp"
#include "neoip_router_rootca_nunit.hpp"
#include "neoip_router_peer_nunit.hpp"
#include "neoip_router_acache_nunit.hpp"
#include "neoip_router_acl_nunit.hpp"
#include "neoip_router_name_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void router_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	router_suite	= nipmem_new nunit_suite_t("router");	
	// log to debug
	KLOG_DBG("enter");

/********************* router_lident_t	***************************************/
	// init the testclass for the router_lident_t
	nunit_testclass_t<router_lident_testclass_t> *	router_lident_testclass;
	router_lident_testclass	= nipmem_new nunit_testclass_t<router_lident_testclass_t>("lident"
							, nipmem_new router_lident_testclass_t());
	// add some test functions
	router_lident_testclass->append("generation"			, &router_lident_testclass_t::generation);
	router_lident_testclass->append("canonical_str_consistency"	, &router_lident_testclass_t::canonical_str_consistency);
	router_lident_testclass->append("signature_consistency"		, &router_lident_testclass_t::signature_consistency);
	// add the testclass to the nunit_suite
	router_suite->append(router_lident_testclass);
	
/********************* router_rident_t	***************************************/
	// init the testclass for the router_rident_t
	nunit_testclass_t<router_rident_testclass_t> *	router_rident_testclass;
	router_rident_testclass	= nipmem_new nunit_testclass_t<router_rident_testclass_t>("rident"
							, nipmem_new router_rident_testclass_t());
	// add some test functions
	router_rident_testclass->append("canonical_str_consistency"	, &router_rident_testclass_t::canonical_str_consistency);
	// add the testclass to the nunit_suite
	router_suite->append(router_rident_testclass);

/********************* router_rootca_t	***************************************/
	// init the testclass for the router_rootca_t
	nunit_testclass_t<router_rootca_testclass_t> *	router_rootca_testclass;
	router_rootca_testclass	= nipmem_new nunit_testclass_t<router_rootca_testclass_t>("rootca"
							, nipmem_new router_rootca_testclass_t());
	// add some test functions
	router_rootca_testclass->append("canonical_str_consistency"	, &router_rootca_testclass_t::canonical_str_consistency);
	// add the testclass to the nunit_suite
	router_suite->append(router_rootca_testclass);
	
/********************* router_acache_t	***************************************/
	// init the testclass for the router_acache_t
	nunit_testclass_t<router_acache_testclass_t> *	router_acache_testclass;
	router_acache_testclass	= nipmem_new nunit_testclass_t<router_acache_testclass_t>("acache"
							, nipmem_new router_acache_testclass_t());
	// add some test functions
	router_acache_testclass->append("item_presence"		, &router_acache_testclass_t::item_presence);
	router_acache_testclass->append("item_expiration"	, &router_acache_testclass_t::item_expiration);
	router_acache_testclass->append("serial_consistency"	, &router_acache_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	router_suite->append(router_acache_testclass);

/********************* router_acl_t	***************************************/
	// init the testclass for the router_acl_t
	nunit_testclass_t<router_acl_testclass_t> *	router_acl_testclass;
	router_acl_testclass	= nipmem_new nunit_testclass_t<router_acl_testclass_t>("acl"
							, nipmem_new router_acl_testclass_t());
	// add some test functions
	router_acl_testclass->append("general"		, &router_acl_testclass_t::general);
	// add the testclass to the nunit_suite
	router_suite->append(router_acl_testclass);
	
/********************* router_peer_t	***************************************/
	// init the testclass for the router_peer_t
	nunit_testclass_t<router_peer_testclass_t> *	router_peer_testclass;
	router_peer_testclass	= nipmem_new nunit_testclass_t<router_peer_testclass_t>("peer"
							, nipmem_new router_peer_testclass_t());
	// add some test functions
	router_peer_testclass->append("wait_for_bstrap"	, &router_peer_testclass_t::wait_for_bstrap);
	router_peer_testclass->append("general"		, &router_peer_testclass_t::general);
	// add the testclass to the nunit_suite
	router_suite->append(router_peer_testclass);


/********************* router_name_t	***************************************/
	// init the testclass for the router_name_t
	nunit_testclass_t<router_name_testclass_t> *	router_name_testclass;
	router_name_testclass	= nipmem_new nunit_testclass_t<router_name_testclass_t>("name"
							, nipmem_new router_name_testclass_t());
	// add some test functions
	router_name_testclass->append("general", &router_name_testclass_t::general);
	// add the testclass to the nunit_suite
	router_suite->append(router_name_testclass);
	
	// add the router_suite to the nunit_suite
	nunit_suite.append(router_suite);
}

// definition of the nunit_gsuite_t for router
NUNIT_GSUITE_DEFINITION(router_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(router_nunit_gsuite, 0, router_gsuite_fct);

NEOIP_NAMESPACE_END





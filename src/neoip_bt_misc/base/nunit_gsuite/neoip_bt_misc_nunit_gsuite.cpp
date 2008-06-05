/*! \file
    \brief Definition of the nunit_gsuite_t for the bt_tracker_t layer

*/

/* system include */
/* local include */
#include "neoip_bt_misc_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_bt_tracker_client_nunit.hpp"
#include "neoip_bt_tracker_server_nunit.hpp"
#include "neoip_bt_mfile_cpuhash_nunit.hpp"
#include "neoip_bt_lnk2mfile_nunit.hpp"
#include "neoip_bt_alloc_nunit.hpp"
#include "neoip_bt_check_nunit.hpp"
#include "neoip_bt_jamstd_nunit.hpp"
#include "neoip_bt_jamrc4_nunit.hpp"
#include "neoip_mlink_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void bt_misc_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	bt_misc_suite	= nipmem_new nunit_suite_t("bt_misc");	
	// log to debug
	KLOG_DBG("enter");

#if 0	// NOTE: to rewrite the nunit 
/********************* bt_mfile_cpuhash_t	***************************************/
	// init the testclass for the bt_mfile_cpuhash_t
	nunit_testclass_t<bt_mfile_cpuhash_testclass_t> *	bt_mfile_cpuhash_testclass;
	bt_mfile_cpuhash_testclass	= nipmem_new nunit_testclass_t<bt_mfile_cpuhash_testclass_t>("mfile_cpuhash"
							, nipmem_new bt_mfile_cpuhash_testclass_t());
	// add some test functions
	bt_mfile_cpuhash_testclass->append("general"	, &bt_mfile_cpuhash_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_misc_suite->append(bt_mfile_cpuhash_testclass);
#endif

/********************* bt_lnk2mfile_t	***************************************/
	// init the testclass for the bt_lnk2mfile_t
	nunit_testclass_t<bt_lnk2mfile_testclass_t> *	bt_lnk2mfile_testclass;
	bt_lnk2mfile_testclass	= nipmem_new nunit_testclass_t<bt_lnk2mfile_testclass_t>("lnk2mfile"
							, nipmem_new bt_lnk2mfile_testclass_t());
	// add some test functions
	bt_lnk2mfile_testclass->append("general"	, &bt_lnk2mfile_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_misc_suite->append(bt_lnk2mfile_testclass);

#if 1
/********************* bt_alloc_t	***************************************/
	// init the testclass for the bt_alloc_t
	nunit_testclass_t<bt_alloc_testclass_t> *	bt_alloc_testclass;
	bt_alloc_testclass	= nipmem_new nunit_testclass_t<bt_alloc_testclass_t>("alloc"
							, nipmem_new bt_alloc_testclass_t());
	// add some test functions
	bt_alloc_testclass->append("general"	, &bt_alloc_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_misc_suite->append(bt_alloc_testclass);
#endif
#if 0
/********************* bt_check_t	***************************************/
	// init the testclass for the bt_check_t
	nunit_testclass_t<bt_check_testclass_t> *	bt_check_testclass;
	bt_check_testclass	= nipmem_new nunit_testclass_t<bt_check_testclass_t>("check"
							, nipmem_new bt_check_testclass_t());
	// add some test functions
	bt_check_testclass->append("general"	, &bt_check_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_misc_suite->append(bt_check_testclass);
#endif
/********************* bt_tracker_client_t	***************************************/
	// init the testclass for the bt_tracker_client_t
	nunit_testclass_t<bt_tracker_client_testclass_t> *	bt_tracker_client_testclass;
	bt_tracker_client_testclass	= nipmem_new nunit_testclass_t<bt_tracker_client_testclass_t>("tracker_client"
							, nipmem_new bt_tracker_client_testclass_t());
	// add some test functions
	bt_tracker_client_testclass->append("general"	, &bt_tracker_client_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_misc_suite->append(bt_tracker_client_testclass);
	
/********************* bt_tracker_server_t	***************************************/
	// init the testclass for the bt_tracker_server_t
	nunit_testclass_t<bt_tracker_server_testclass_t> *	bt_tracker_server_testclass;
	bt_tracker_server_testclass	= nipmem_new nunit_testclass_t<bt_tracker_server_testclass_t>("tracker_server"
							, nipmem_new bt_tracker_server_testclass_t());
	// add some test functions
	bt_tracker_server_testclass->append("general"	, &bt_tracker_server_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_misc_suite->append(bt_tracker_server_testclass);
	
/********************* bt_jamstd_t	***************************************/
	// init the testclass for the bt_jamstd_t
	nunit_testclass_t<bt_jamstd_testclass_t> *	bt_jamstd_testclass;
	bt_jamstd_testclass	= nipmem_new nunit_testclass_t<bt_jamstd_testclass_t>("jamstd"
							, nipmem_new bt_jamstd_testclass_t());
	// add some test functions
	bt_jamstd_testclass->append("connection_esta"		, &bt_jamstd_testclass_t::connection_esta);
	bt_jamstd_testclass->append("resp_detect_plainbt"	, &bt_jamstd_testclass_t::resp_detect_plainbt);
	// add the testclass to the nunit_suite
	bt_misc_suite->append(bt_jamstd_testclass);
	
/********************* bt_jamrc4_t	***************************************/
	// init the testclass for the bt_jamrc4_t
	nunit_testclass_t<bt_jamrc4_testclass_t> *	bt_jamrc4_testclass;
	bt_jamrc4_testclass	= nipmem_new nunit_testclass_t<bt_jamrc4_testclass_t>("jamrc4"
							, nipmem_new bt_jamrc4_testclass_t());
	// add some test functions
	bt_jamrc4_testclass->append("cnx_nojam2nojam"	, &bt_jamrc4_testclass_t::cnx_nojam2nojam);
	bt_jamrc4_testclass->append("cnx_dojam2dojam"	, &bt_jamrc4_testclass_t::cnx_dojam2dojam);
	bt_jamrc4_testclass->append("cnx_nojam2dojam"	, &bt_jamrc4_testclass_t::cnx_nojam2dojam);
	bt_jamrc4_testclass->append("cnx_nojam2anyjam"	, &bt_jamrc4_testclass_t::cnx_nojam2anyjam);
	// add the testclass to the nunit_suite
	bt_misc_suite->append(bt_jamrc4_testclass);

/********************* mlink_t	***************************************/
	// init the testclass for the mlink_t
	nunit_testclass_t<mlink_testclass_t> *	mlink_testclass;
	mlink_testclass	= nipmem_new nunit_testclass_t<mlink_testclass_t>("mlink"
							, nipmem_new mlink_testclass_t());
	// add some test functions
	mlink_testclass->append("general"	, &mlink_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_misc_suite->append(mlink_testclass);

	// add the bt_misc_suite to the nunit_suite
	nunit_suite.append(bt_misc_suite);
}

// definition of the nunit_gsuite_t for bt_tracker
NUNIT_GSUITE_DEFINITION(bt_misc_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(bt_misc_nunit_gsuite, 0, bt_misc_gsuite_fct);

NEOIP_NAMESPACE_END





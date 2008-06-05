/*! \file
    \brief Definition of the nunit_gsuite_t for the bt_tracker_t layer

*/

/* system include */
/* local include */
#include "neoip_bt_core_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// list of #include for each testclass
#include "neoip_bt_tracker_client_nunit.hpp"
#include "neoip_bt_tracker_server_nunit.hpp"
#include "neoip_bt_mfile_nunit.hpp"
#include "neoip_bt_mfile_cpuhash_nunit.hpp"
#include "neoip_bt_lnk2mfile_nunit.hpp"
#include "neoip_bt_alloc_nunit.hpp"
#include "neoip_bt_check_nunit.hpp"
#include "neoip_bt_swarm_nunit.hpp"
#include "neoip_bt_peerid_helper_nunit.hpp"
#include "neoip_bt_prange_nunit.hpp"
#include "neoip_bt_pieceavail_nunit.hpp"
#include "neoip_bt_pieceprec_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void bt_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	bt_core_suite	= nipmem_new nunit_suite_t("bt_core");	
	// log to debug
	KLOG_DBG("enter");

/********************* bt_mfile_t	***************************************/
	// init the testclass for the bt_mfile_t
	nunit_testclass_t<bt_mfile_testclass_t> *	bt_mfile_testclass;
	bt_mfile_testclass	= nipmem_new nunit_testclass_t<bt_mfile_testclass_t>("mfile"
							, nipmem_new bt_mfile_testclass_t());
	// add some test functions
	bt_mfile_testclass->append("general"	, &bt_mfile_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_mfile_testclass);

#if 0	// NOTE: to rewrite the nunit 
/********************* bt_mfile_cpuhash_t	***************************************/
	// init the testclass for the bt_mfile_cpuhash_t
	nunit_testclass_t<bt_mfile_cpuhash_testclass_t> *	bt_mfile_cpuhash_testclass;
	bt_mfile_cpuhash_testclass	= nipmem_new nunit_testclass_t<bt_mfile_cpuhash_testclass_t>("mfile_cpuhash"
							, nipmem_new bt_mfile_cpuhash_testclass_t());
	// add some test functions
	bt_mfile_cpuhash_testclass->append("general"	, &bt_mfile_cpuhash_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_mfile_cpuhash_testclass);
#endif

/********************* bt_lnk2mfile_t	***************************************/
	// init the testclass for the bt_lnk2mfile_t
	nunit_testclass_t<bt_lnk2mfile_testclass_t> *	bt_lnk2mfile_testclass;
	bt_lnk2mfile_testclass	= nipmem_new nunit_testclass_t<bt_lnk2mfile_testclass_t>("lnk2mfile"
							, nipmem_new bt_lnk2mfile_testclass_t());
	// add some test functions
	bt_lnk2mfile_testclass->append("general"	, &bt_lnk2mfile_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_lnk2mfile_testclass);

#if 0
/********************* bt_alloc_t	***************************************/
	// init the testclass for the bt_alloc_t
	nunit_testclass_t<bt_alloc_testclass_t> *	bt_alloc_testclass;
	bt_alloc_testclass	= nipmem_new nunit_testclass_t<bt_alloc_testclass_t>("alloc"
							, nipmem_new bt_alloc_testclass_t());
	// add some test functions
	bt_alloc_testclass->append("general"	, &bt_alloc_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_alloc_testclass);
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
	bt_core_suite->append(bt_check_testclass);
#endif
/********************* bt_tracker_client_t	***************************************/
	// init the testclass for the bt_tracker_client_t
	nunit_testclass_t<bt_tracker_client_testclass_t> *	bt_tracker_client_testclass;
	bt_tracker_client_testclass	= nipmem_new nunit_testclass_t<bt_tracker_client_testclass_t>("tracker_client"
							, nipmem_new bt_tracker_client_testclass_t());
	// add some test functions
	bt_tracker_client_testclass->append("general"	, &bt_tracker_client_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_tracker_client_testclass);
	
/********************* bt_tracker_server_t	***************************************/
	// init the testclass for the bt_tracker_server_t
	nunit_testclass_t<bt_tracker_server_testclass_t> *	bt_tracker_server_testclass;
	bt_tracker_server_testclass	= nipmem_new nunit_testclass_t<bt_tracker_server_testclass_t>("tracker_server"
							, nipmem_new bt_tracker_server_testclass_t());
	// add some test functions
	bt_tracker_server_testclass->append("general"	, &bt_tracker_server_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_tracker_server_testclass);

/********************* bt_peerid_helper_t	***************************************/
	// init the testclass for the bt_peerid_helper_t
	nunit_testclass_t<bt_peerid_helper_testclass_t> *	bt_peerid_helper_testclass;
	bt_peerid_helper_testclass	= nipmem_new nunit_testclass_t<bt_peerid_helper_testclass_t>("peerid_helper"
							, nipmem_new bt_peerid_helper_testclass_t());
	// add some test functions
	bt_peerid_helper_testclass->append("general"	, &bt_peerid_helper_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_peerid_helper_testclass);

/********************* bt_pieceavail_t	***************************************/
	// init the testclass for the bt_pieceavail_t
	nunit_testclass_t<bt_pieceavail_testclass_t> *	bt_pieceavail_testclass;
	bt_pieceavail_testclass	= nipmem_new nunit_testclass_t<bt_pieceavail_testclass_t>("pieceavail"
							, nipmem_new bt_pieceavail_testclass_t());
	// add some test functions
	bt_pieceavail_testclass->append("general"	, &bt_pieceavail_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_pieceavail_testclass);

/********************* bt_prange_t	***************************************/
	// init the testclass for the bt_prange_t
	nunit_testclass_t<bt_prange_testclass_t> *	bt_prange_testclass;
	bt_prange_testclass	= nipmem_new nunit_testclass_t<bt_prange_testclass_t>("prange"
							, nipmem_new bt_prange_testclass_t());
	// add some test functions
	bt_prange_testclass->append("general"		, &bt_prange_testclass_t::general);
	bt_prange_testclass->append("comparison"	, &bt_prange_testclass_t::comparison);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_prange_testclass);

/********************* bt_pieceprec_t	***************************************/
	// init the testclass for the bt_pieceprec_t
	nunit_testclass_t<bt_pieceprec_testclass_t> *	bt_pieceprec_testclass;
	bt_pieceprec_testclass	= nipmem_new nunit_testclass_t<bt_pieceprec_testclass_t>("pieceprec"
							, nipmem_new bt_pieceprec_testclass_t());
	// add some test functions
	bt_pieceprec_testclass->append("general"		, &bt_pieceprec_testclass_t::general);
	bt_pieceprec_testclass->append("array"			, &bt_pieceprec_testclass_t::array);
	bt_pieceprec_testclass->append("serial_consistency"	, &bt_pieceprec_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_pieceprec_testclass);

#if 0
/********************* bt_swarm_t	***************************************/
	// init the testclass for the bt_swarm_t
	nunit_testclass_t<bt_swarm_testclass_t> *	bt_swarm_testclass;
	bt_swarm_testclass	= nipmem_new nunit_testclass_t<bt_swarm_testclass_t>("swarm"
							, nipmem_new bt_swarm_testclass_t());
	// add some test functions
	bt_swarm_testclass->append("general"	, &bt_swarm_testclass_t::general);
	// add the testclass to the nunit_suite
	bt_core_suite->append(bt_swarm_testclass);
#endif


	// add the bt_core_suite to the nunit_suite
	nunit_suite.append(bt_core_suite);
}

// definition of the nunit_gsuite_t for bt_tracker
NUNIT_GSUITE_DEFINITION(bt_core_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(bt_core_nunit_gsuite, 0, bt_gsuite_fct);

NEOIP_NAMESPACE_END





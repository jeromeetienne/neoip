/*! \file
    \brief Definition of the nunit_suite_t

*/

/* system include */
/* local include */
#include "neoip_kad_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_kad_client_rpc_nunit.hpp"
#include "neoip_kad_bstrap_src_nunit.hpp"
#include "neoip_kad_cmd_nunit.hpp"
#include "neoip_kad_publish_nunit.hpp"
#include "neoip_kad_rec_src_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit_gsuite_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Implement a nunit_gsuite_fct_t for the kad layer
 */
static void kad_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	kad_suite	= nipmem_new nunit_suite_t("kad");		
	// log to debug
	KLOG_DBG("enter");



/********************* kad_client_rpc_t	***************************************/
	// init the testclass for the kad_client_rpc_t
	nunit_testclass_t<kad_client_rpc_testclass_t> *	kad_client_rpc_testclass;
	kad_client_rpc_testclass = nipmem_new nunit_testclass_t<kad_client_rpc_testclass_t>("kad_client_rpc"
							, nipmem_new kad_client_rpc_testclass_t());
	// add some test functions
	kad_client_rpc_testclass->append("test_ping_doreach_peer"	, &kad_client_rpc_testclass_t::test_ping_doreach_peer);
	kad_client_rpc_testclass->append("test_ping_unreach_peer"	, &kad_client_rpc_testclass_t::test_ping_unreach_peer);
	kad_client_rpc_testclass->append("test_findnode"		, &kad_client_rpc_testclass_t::test_findnode);
	kad_client_rpc_testclass->append("test_findsomeval_fornode"	, &kad_client_rpc_testclass_t::test_findsomeval_fornode);
	kad_client_rpc_testclass->append("test_store"			, &kad_client_rpc_testclass_t::test_store);
	kad_client_rpc_testclass->append("test_findsomeval_forval"	, &kad_client_rpc_testclass_t::test_findsomeval_forval);
	kad_client_rpc_testclass->append("test_findallval_do_found"	, &kad_client_rpc_testclass_t::test_findallval_do_found);
	kad_client_rpc_testclass->append("test_delete_record"		, &kad_client_rpc_testclass_t::test_delete_record);
	kad_client_rpc_testclass->append("test_findallval_notfound"	, &kad_client_rpc_testclass_t::test_findallval_notfound);
	// add the testclass to the nunit_suite
	kad_suite->append(kad_client_rpc_testclass);	

/********************* kad_bstrap_src_t	***************************************/
	// init the testclass for the kad_bstrap_src_t
	nunit_testclass_t<kad_bstrap_src_testclass_t> *	kad_bstrap_src_testclass;
	kad_bstrap_src_testclass = nipmem_new nunit_testclass_t<kad_bstrap_src_testclass_t>("kad_bstrap_src"
							, nipmem_new kad_bstrap_src_testclass_t());
	// add some test functions
	kad_bstrap_src_testclass->append("general"		, &kad_bstrap_src_testclass_t::general);
	// add the testclass to the nunit_suite
	kad_suite->append(kad_bstrap_src_testclass);

/********************* kad_cmd_t	***************************************/
	// init the testclass for the kad_cmd_t
	nunit_testclass_t<kad_cmd_testclass_t> *	kad_cmd_testclass;
	kad_cmd_testclass = nipmem_new nunit_testclass_t<kad_cmd_testclass_t>("cmd"
							, nipmem_new kad_cmd_testclass_t());
	// add some test functions
	kad_cmd_testclass->append("wait_for_bstrap"	, &kad_cmd_testclass_t::wait_for_bstrap);
	kad_cmd_testclass->append("store_record"	, &kad_cmd_testclass_t::store_record);
	kad_cmd_testclass->append("query_some_dofound"	, &kad_cmd_testclass_t::query_some_dofound);
	kad_cmd_testclass->append("query_dofound"	, &kad_cmd_testclass_t::query_dofound);
	kad_cmd_testclass->append("delete_record"	, &kad_cmd_testclass_t::delete_record);
	kad_cmd_testclass->append("query_some_nofound"	, &kad_cmd_testclass_t::query_some_nofound);
	kad_cmd_testclass->append("query_nofound"	, &kad_cmd_testclass_t::query_nofound);
	kad_cmd_testclass->append("closestnode"		, &kad_cmd_testclass_t::closestnode);
	// add the testclass to the nunit_suite
	kad_suite->append(kad_cmd_testclass);	


/********************* kad_publish_t	***************************************/
	// init the testclass for the kad_publish_t
	nunit_testclass_t<kad_publish_testclass_t> *	kad_publish_testclass;
	kad_publish_testclass = nipmem_new nunit_testclass_t<kad_publish_testclass_t>("kad_publish"
							, nipmem_new kad_publish_testclass_t());
	// add some test functions
	kad_publish_testclass->append("test_publish_start"	, &kad_publish_testclass_t::test_publish_start);
	kad_publish_testclass->append("test_query_dofound"	, &kad_publish_testclass_t::test_query_dofound);
	kad_publish_testclass->append("test_publish_stop"	, &kad_publish_testclass_t::test_publish_stop);
// TODO disable as the deleting of kad_publish_t has been disabled as it trigger a BIG BUG
// FIXME BUG BUG BUG TODO
//	kad_publish_testclass->append("test_query_nofound"	, &kad_publish_testclass_t::test_query_nofound);
	// add the testclass to the nunit_suite
	kad_suite->append(kad_publish_testclass);	

/********************* kad_rec_src_t	***************************************/
	// init the testclass for the kad_rec_src_t
	nunit_testclass_t<kad_rec_src_testclass_t> *	kad_rec_src_testclass;
	kad_rec_src_testclass	= nipmem_new nunit_testclass_t<kad_rec_src_testclass_t>("kad_rec_src_t"
							, nipmem_new kad_rec_src_testclass_t());
	// add some test functions
	kad_rec_src_testclass->append("store_record"	, &kad_rec_src_testclass_t::store_record);
	kad_rec_src_testclass->append("general"		, &kad_rec_src_testclass_t::general);
	// add the testclass to the nunit_suite
	kad_suite->append(kad_rec_src_testclass);

	// add the kad_suite to the nunit_suite
	nunit_suite.append(kad_suite);		
}

// definition of the nunit_gsuite_t for kad
NUNIT_GSUITE_DEFINITION(kad_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(kad_nunit_gsuite, 0, kad_gsuite_fct);

NEOIP_NAMESPACE_END





/*! \file
    \brief Definition of the nunit_suite_t

*/

/* system include */
/* local include */
#include "neoip_netutils_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_gen_id_nunit.hpp"
#include "neoip_bfilter_nunit.hpp"
#include "neoip_bencode_nunit.hpp"
#include "neoip_mimediag_nunit.hpp"
#include "neoip_rate_sched_nunit.hpp"
#include "neoip_rate_estim_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit_gsuite_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Implement a nunit_gsuite_fct_t for the kad layer
 */
static void netutils_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	netutils_suite	= nipmem_new nunit_suite_t("netutils");	
	// log to debug
	KLOG_DBG("enter");
/********************* gen_id_t	***************************************/
	// init the testclass for the gen_id_t
	nunit_testclass_t<gen_id_testclass_t> *	gen_id_testclass;
	gen_id_testclass	= nipmem_new nunit_testclass_t<gen_id_testclass_t>("gen_id", nipmem_new gen_id_testclass_t());
	// add some test functions
	gen_id_testclass->append("test_is_null"			, &gen_id_testclass_t::test_is_null);
	gen_id_testclass->append("serial_consistency"		, &gen_id_testclass_t::serial_consistency);
	gen_id_testclass->append("xmlrpc_consistency"		, &gen_id_testclass_t::xmlrpc_consistency);
	gen_id_testclass->append("canonical_str_consistency"	, &gen_id_testclass_t::canonical_str_consistency);
	gen_id_testclass->append("derivation_hash"		, &gen_id_testclass_t::derivation_hash);
	gen_id_testclass->append("bit_clear_n_highest"		, &gen_id_testclass_t::bit_clear_n_highest);
	gen_id_testclass->append("bit_get_highest_set"		, &gen_id_testclass_t::bit_get_highest_set);
	gen_id_testclass->append("test_xor"			, &gen_id_testclass_t::test_xor);
	gen_id_testclass->append("test_and"			, &gen_id_testclass_t::test_and);
	gen_id_testclass->append("test_rshift"			, &gen_id_testclass_t::test_rshift);
	gen_id_testclass->append("comparison_op"		, &gen_id_testclass_t::comparison_op);
	// add the testclass to the nunit_suite
	netutils_suite->append(gen_id_testclass);

/********************* bfilter_t	***************************************/
	// init the testclass for the bfilter_t
	nunit_testclass_t<bfilter_testclass_t> *	bfilter_testclass;
	bfilter_testclass	= nipmem_new nunit_testclass_t<bfilter_testclass_t>("bfilter_t"
							, nipmem_new bfilter_testclass_t());
	// add some test functions
	bfilter_testclass->append("insert"		, &bfilter_testclass_t::insert);
	bfilter_testclass->append("unify"		, &bfilter_testclass_t::unify);
	bfilter_testclass->append("intersect"		, &bfilter_testclass_t::intersect);
	// add the testclass to the nunit_suite
	netutils_suite->append(bfilter_testclass);	

/********************* bencode_t	***************************************/
	// init the testclass for the bencode_t
	nunit_testclass_t<bencode_testclass_t> *	bencode_testclass;
	bencode_testclass	= nipmem_new nunit_testclass_t<bencode_testclass_t>("bencode"
							, nipmem_new bencode_testclass_t());
	// add some test functions
	bencode_testclass->append("encode"	, &bencode_testclass_t::encode);
	bencode_testclass->append("decode"	, &bencode_testclass_t::decode);
	bencode_testclass->append("consistency"	, &bencode_testclass_t::consistency);
	// add the testclass to the nunit_suite
	netutils_suite->append(bencode_testclass);	
		

/********************* mimediag_t	***************************************/
	// init the testclass for the mimediag_t
	nunit_testclass_t<mimediag_testclass_t> *	mimediag_testclass;
	mimediag_testclass	= nipmem_new nunit_testclass_t<mimediag_testclass_t>("mimediag"
							, nipmem_new mimediag_testclass_t());
	// add some test functions
	mimediag_testclass->append("general"	, &mimediag_testclass_t::general);
	// add the testclass to the nunit_suite
	netutils_suite->append(mimediag_testclass);	

#if 0	// TODO to put a real nunit and reenable - the current stuff is never ending
/********************* rate2_sched_t	***************************************/
	// init the testclass for the rate2_sched_t
	nunit_testclass_t<rate_sched_testclass_t> *	rate_sched_testclass;
	rate_sched_testclass	= nipmem_new nunit_testclass_t<rate_sched_testclass_t>("rate_sched"
							, nipmem_new rate_sched_testclass_t());
	// add some test functions
	rate_sched_testclass->append("general"	, &rate_sched_testclass_t::general);
	// add the testclass to the nunit_suite
	netutils_suite->append(rate_sched_testclass);	
#endif

/********************* rate_estim_t	***************************************/
	// init the testclass for the rate_estim_t
	nunit_testclass_t<rate_estim_testclass_t> *	rate_estim_testclass;
	rate_estim_testclass	= nipmem_new nunit_testclass_t<rate_estim_testclass_t>("rate_estim"
							, nipmem_new rate_estim_testclass_t());
	// add some test functions
	rate_estim_testclass->append("general"	, &rate_estim_testclass_t::general);
	// add the testclass to the nunit_suite
	netutils_suite->append(rate_estim_testclass);	

	// add the netutils_suite to the nunit_suite
	nunit_suite.append(netutils_suite);	
}

// definition of the nunit_gsuite_t for kad
NUNIT_GSUITE_DEFINITION(netutils_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(netutils_nunit_gsuite, 0, netutils_gsuite_fct);

NEOIP_NAMESPACE_END





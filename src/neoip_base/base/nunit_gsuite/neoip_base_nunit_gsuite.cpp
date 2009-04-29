/*! \file
    \brief Definition of the nunit_suite_t

*/

/* system include */
/* local include */
#include "neoip_base_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

// include for the nunit testclass
#include "neoip_nipmem_nunit.hpp"
#include "neoip_obj_factory_nunit.hpp"
#include "neoip_timeout_nunit.hpp"
#include "neoip_delaygen_nunit.hpp"
#include "neoip_slotpool_nunit.hpp"
#include "neoip_object_slotid_nunit.hpp"
#include "neoip_tokeep_check_nunit.hpp"
#include "neoip_object_slotid.hpp"
#include "neoip_event_hook_nunit.hpp"
#include "neoip_errtype_nunit.hpp"
#include "neoip_strtype_nunit.hpp"
#include "neoip_pkttype_nunit.hpp"
#include "neoip_expireset_nunit.hpp"
#include "neoip_expireset2_nunit.hpp"
#include "neoip_item_arr_nunit.hpp"
#include "neoip_bitfield_nunit.hpp"
#include "neoip_bitcount_nunit.hpp"
#include "neoip_base64_nunit.hpp"
#include "neoip_interval_nunit.hpp"
#include "neoip_dvar_nunit.hpp"
#include "neoip_ptr_nunit.hpp"
#include "neoip_profile_nunit.hpp"
#include "neoip_strvar_db_nunit.hpp"
#include "neoip_datum_nunit.hpp"
#include "neoip_serial_nunit.hpp"
#include "neoip_delay_nunit.hpp"
#include "neoip_date_nunit.hpp"
#include "neoip_bitflag_nunit.hpp"
#include "neoip_string_nunit.hpp"
#include "neoip_mlist_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void base_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	base_suite	= nipmem_new nunit_suite_t("base");
	// log to debug
	KLOG_DBG("enter");

#if 0	// NOTE: no real nunit - it is more a testbed handled manually when i code nipmem
/********************* nipmem_t	***************************************/
	// init the testclass for the nipmem_t
	nunit_testclass_t<nipmem_testclass_t> *	nipmem_testclass;
	nipmem_testclass	= nipmem_new nunit_testclass_t<nipmem_testclass_t>("nipmem"
							, nipmem_new nipmem_testclass_t());
	// add some test functions
	nipmem_testclass->append("general"	, &nipmem_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(nipmem_testclass);
#endif

	/********************* obj_factory_t	***************************************/
	// init the testclass for the obj_factory_t
	nunit_testclass_t<obj_factory_testclass_t> *	obj_factory_testclass;
	obj_factory_testclass	= nipmem_new nunit_testclass_t<obj_factory_testclass_t>("obj_factory"
							, nipmem_new obj_factory_testclass_t());
	// add some test functions
	obj_factory_testclass->append("general"	, &obj_factory_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(obj_factory_testclass);

/********************* delaygen_t	***************************************/
	// init the testclass for the delaygen_t
	nunit_testclass_t<delaygen_testclass_t> *	delaygen_testclass;
	delaygen_testclass	= nipmem_new nunit_testclass_t<delaygen_testclass_t>("delaygen"
							, nipmem_new delaygen_testclass_t());
	// add some test functions
	delaygen_testclass->append("delaygen_arg"	, &delaygen_testclass_t::delaygen_arg);
	delaygen_testclass->append("regular_nonrandom"	, &delaygen_testclass_t::regular_nonrandom);
	delaygen_testclass->append("regular_random"	, &delaygen_testclass_t::regular_random);
	delaygen_testclass->append("expboff_nonrandom"	, &delaygen_testclass_t::expboff_nonrandom);
	delaygen_testclass->append("expboff_random"	, &delaygen_testclass_t::expboff_random);
	// add the testclass to the nunit_suite
	base_suite->append(delaygen_testclass);

/********************* slotpool_t	***************************************/
	// init the testclass for the slotpool_t
	nunit_testclass_t<slotpool_testclass_t> *	slotpool_testclass;
	slotpool_testclass	= nipmem_new nunit_testclass_t<slotpool_testclass_t>("slotpool"
							, nipmem_new slotpool_testclass_t());
	// add some test functions
	slotpool_testclass->append("general"	, &slotpool_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(slotpool_testclass);


/********************* tokeep_check_t	***************************************/
	// init the testclass for the tokeep_check_t
	nunit_testclass_t<tokeep_check_testclass_t> *	tokeep_check_testclass;
	tokeep_check_testclass	= nipmem_new nunit_testclass_t<tokeep_check_testclass_t>("tokeep_check"
							, nipmem_new tokeep_check_testclass_t());
	// add some test functions
	tokeep_check_testclass->append("using_internal"	, &tokeep_check_testclass_t::using_internal);
	tokeep_check_testclass->append("using_define"	, &tokeep_check_testclass_t::using_define);
	tokeep_check_testclass->append("copy_operator"	, &tokeep_check_testclass_t::copy_operator);
	tokeep_check_testclass->append("assignement"	, &tokeep_check_testclass_t::assignement);
	// add the testclass to the nunit_suite
	base_suite->append(tokeep_check_testclass);

/********************* object_slotid_t	***************************************/
	// init the testclass for the object_slotid_t
	nunit_testclass_t<object_slotid_testclass_t> *	object_slotid_testclass;
	object_slotid_testclass	= nipmem_new nunit_testclass_t<object_slotid_testclass_t>("object_slotid"
							, nipmem_new object_slotid_testclass_t());
	// add some test functions
	object_slotid_testclass->append("using_tokeep"	, &object_slotid_testclass_t::using_tokeep);
	object_slotid_testclass->append("copy_operator"	, &object_slotid_testclass_t::copy_operator);
	object_slotid_testclass->append("normal_assignement"	, &object_slotid_testclass_t::normal_assignement);
	object_slotid_testclass->append("self_assignement"	, &object_slotid_testclass_t::self_assignement);
	// add the testclass to the nunit_suite
	base_suite->append(object_slotid_testclass);

/********************* event_hook_t	***************************************/
	// init the testclass for the event_hook_t
	nunit_testclass_t<event_hook_testclass_t> *	event_hook_testclass;
	event_hook_testclass	= nipmem_new nunit_testclass_t<event_hook_testclass_t>("event_hook"
							, nipmem_new event_hook_testclass_t());
	// add some test functions
	event_hook_testclass->append("general"	, &event_hook_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(event_hook_testclass);

/********************* errtype_t	***************************************/
	// init the testclass for the errtype_t
	nunit_testclass_t<errtype_testclass_t> *	errtype_testclass;
	errtype_testclass	= nipmem_new nunit_testclass_t<errtype_testclass_t>("errtype"
							, nipmem_new errtype_testclass_t());
	// add some test functions
	errtype_testclass->append("general"	, &errtype_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(errtype_testclass);


/********************* strtype_t	***************************************/
	// init the testclass for the strtype_t
	nunit_testclass_t<strtype_testclass_t> *	strtype_testclass;
	strtype_testclass	= nipmem_new nunit_testclass_t<strtype_testclass_t>("strtype"
							, nipmem_new strtype_testclass_t());
	// add some test functions
	strtype_testclass->append("general"	, &strtype_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(strtype_testclass);

/********************* pkttype_t	***************************************/
	// init the testclass for the pkttype_t
	nunit_testclass_t<pkttype_testclass_t> *	pkttype_testclass;
	pkttype_testclass	= nipmem_new nunit_testclass_t<pkttype_testclass_t>("pkttype"
							, nipmem_new pkttype_testclass_t());
	// add some test functions
	pkttype_testclass->append("test_null"		, &pkttype_testclass_t::test_null);
	pkttype_testclass->append("comparison"		, &pkttype_testclass_t::comparison);
	pkttype_testclass->append("serial_offset"	, &pkttype_testclass_t::serial_offset);
	pkttype_testclass->append("serial_consistency"	, &pkttype_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	base_suite->append(pkttype_testclass);

/********************* expireset_t	***************************************/
	// init the testclass for the expireset_t
	nunit_testclass_t<expireset_testclass_t> *	expireset_testclass;
	expireset_testclass	= nipmem_new nunit_testclass_t<expireset_testclass_t>("expireset"
							, nipmem_new expireset_testclass_t());
	// add some test functions
	expireset_testclass->append("general"	, &expireset_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(expireset_testclass);

/********************* expireset2_t	***************************************/
	// init the testclass for the expireset2_t
	nunit_testclass_t<expireset2_testclass_t> *	expireset2_testclass;
	expireset2_testclass	= nipmem_new nunit_testclass_t<expireset2_testclass_t>("expireset2"
							, nipmem_new expireset2_testclass_t());
	// add some test functions
	expireset2_testclass->append("general"			, &expireset2_testclass_t::general);
	expireset2_testclass->append("serial_consistency"	, &expireset2_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	base_suite->append(expireset2_testclass);

/********************* item_arr_t	***************************************/
	// init the testclass for the item_arr_t
	nunit_testclass_t<item_arr_testclass_t> *	item_arr_testclass;
	item_arr_testclass	= nipmem_new nunit_testclass_t<item_arr_testclass_t>("item_arr"
							, nipmem_new item_arr_testclass_t());
	// add some test functions
	item_arr_testclass->append("general"		, &item_arr_testclass_t::general);
	item_arr_testclass->append("remove"		, &item_arr_testclass_t::remove);
	item_arr_testclass->append("search_function"	, &item_arr_testclass_t::search_function);
	item_arr_testclass->append("serial_consistency"	, &item_arr_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	base_suite->append(item_arr_testclass);

/********************* bitfield_t	***************************************/
	// init the testclass for the bitfield_t
	nunit_testclass_t<bitfield_testclass_t> *	bitfield_testclass;
	bitfield_testclass	= nipmem_new nunit_testclass_t<bitfield_testclass_t>("bitfield"
							, nipmem_new bitfield_testclass_t());
	// add some test functions
	bitfield_testclass->append("is_null"		, &bitfield_testclass_t::is_null);
	bitfield_testclass->append("to_canonical_string", &bitfield_testclass_t::to_canonical_string);
	bitfield_testclass->append("comparison"		, &bitfield_testclass_t::comparison);
	bitfield_testclass->append("get_next_set_unset"	, &bitfield_testclass_t::get_next_set_unset);
	bitfield_testclass->append("bool_operation"	, &bitfield_testclass_t::bool_operation);
	bitfield_testclass->append("serial_consistency"	, &bitfield_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	base_suite->append(bitfield_testclass);

/********************* bitcount_t	***************************************/
	// init the testclass for the bitcount_t
	nunit_testclass_t<bitcount_testclass_t> *	bitcount_testclass;
	bitcount_testclass	= nipmem_new nunit_testclass_t<bitcount_testclass_t>("bitcount"
							, nipmem_new bitcount_testclass_t());
	// add some test functions
	bitcount_testclass->append("general"		, &bitcount_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(bitcount_testclass);

/********************* base64_t	***************************************/
	// init the testclass for the base64_t
	nunit_testclass_t<base64_testclass_t> *	base64_testclass;
	base64_testclass	= nipmem_new nunit_testclass_t<base64_testclass_t>("base64"
							, nipmem_new base64_testclass_t());
	// add some test functions
	base64_testclass->append("norm_alphabet"	, &base64_testclass_t::norm_alphabet);
	base64_testclass->append("safe_alphabet"	, &base64_testclass_t::safe_alphabet);
	base64_testclass->append("self_consitency"	, &base64_testclass_t::self_consistency);
	// add the testclass to the nunit_suite
	base_suite->append(base64_testclass);

/********************* interval_t	***************************************/
	// init the testclass for the interval_t
	nunit_testclass_t<interval_testclass_t> *	interval_testclass;
	interval_testclass	= nipmem_new nunit_testclass_t<interval_testclass_t>("interval"
							, nipmem_new interval_testclass_t());
	// add some test functions
	interval_testclass->append("item_ctor"			, &interval_testclass_t::item_ctor);
	interval_testclass->append("item_serial_consistency"	, &interval_testclass_t::item_serial_consistency);
	interval_testclass->append("item_is_distinct"		, &interval_testclass_t::item_is_distinct);
	interval_testclass->append("item_fully_include"		, &interval_testclass_t::item_fully_include);
	interval_testclass->append("interval_add_item"		, &interval_testclass_t::interval_add_item);
	interval_testclass->append("interval_sub_item"		, &interval_testclass_t::interval_sub_item);
	interval_testclass->append("interval_serial_consistency", &interval_testclass_t::interval_serial_consistency);
	// add the testclass to the nunit_suite
	base_suite->append(interval_testclass);


/********************* dvar_t	***************************************/
	// init the testclass for the dvar_t
	nunit_testclass_t<dvar_testclass_t> *	dvar_testclass;
	dvar_testclass	= nipmem_new nunit_testclass_t<dvar_testclass_t>("dvar"
							, nipmem_new dvar_testclass_t());
	// add some test functions
	dvar_testclass->append("general"	, &dvar_testclass_t::general);
	dvar_testclass->append("integer"	, &dvar_testclass_t::integer);
	dvar_testclass->append("dbl"		, &dvar_testclass_t::dbl);
	dvar_testclass->append("string"		, &dvar_testclass_t::string);
	dvar_testclass->append("boolean"	, &dvar_testclass_t::boolean);
	dvar_testclass->append("nil"		, &dvar_testclass_t::nil);
	dvar_testclass->append("array"		, &dvar_testclass_t::array);
	dvar_testclass->append("map"		, &dvar_testclass_t::map);
	dvar_testclass->append("to_xml"		, &dvar_testclass_t::to_xml);
	dvar_testclass->append("to_http_query"	, &dvar_testclass_t::to_http_query);
	dvar_testclass->append("from_http_query", &dvar_testclass_t::from_http_query);
	// add the testclass to the nunit_suite
	base_suite->append(dvar_testclass);

/********************* ptr_t	***************************************/
	// init the testclass for the ptr_t
	nunit_testclass_t<ptr_testclass_t> *	ptr_testclass;
	ptr_testclass	= nipmem_new nunit_testclass_t<ptr_testclass_t>("ptr"
							, nipmem_new ptr_testclass_t());
	// add some test functions
	ptr_testclass->append("ptr_counted"	, &ptr_testclass_t::ptr_counted);
	ptr_testclass->append("ptr_cow"		, &ptr_testclass_t::ptr_cow);
	// add the testclass to the nunit_suite
	base_suite->append(ptr_testclass);

/********************* profile_t	***************************************/
	// init the testclass for the profile_t
	nunit_testclass_t<profile_testclass_t> *	profile_testclass;
	profile_testclass	= nipmem_new nunit_testclass_t<profile_testclass_t>("profile"
							, nipmem_new profile_testclass_t());
	// add some test functions
	profile_testclass->append("general"	, &profile_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(profile_testclass);

/********************* strvar_db_t	***************************************/
	// init the testclass for the strvar_db_t
	nunit_testclass_t<strvar_db_testclass_t> *	strvar_db_testclass;
	strvar_db_testclass	= nipmem_new nunit_testclass_t<strvar_db_testclass_t>("strvar_db"
							, nipmem_new strvar_db_testclass_t());
	// add some test functions
	strvar_db_testclass->append("general"	, &strvar_db_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(strvar_db_testclass);

/********************* datum_t	***************************************/
	// init the testclass for the datum_t
	nunit_testclass_t<datum_testclass_t> *	datum_testclass;
	datum_testclass	= nipmem_new nunit_testclass_t<datum_testclass_t>("datum"
							, nipmem_new datum_testclass_t());
	// add some test functions
	datum_testclass->append("general"		, &datum_testclass_t::general);
	datum_testclass->append("test_flag"		, &datum_testclass_t::test_flag);
	datum_testclass->append("serial_consistency"	, &datum_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	base_suite->append(datum_testclass);

/********************* serial_t	***************************************/
	// init the testclass for the serial_t
	nunit_testclass_t<serial_testclass_t> *	serial_testclass;
	serial_testclass	= nipmem_new nunit_testclass_t<serial_testclass_t>("serial"
							, nipmem_new serial_testclass_t());
	// add some test functions
	serial_testclass->append("basic_serial"		, &serial_testclass_t::basic_serial);
	serial_testclass->append("failed_serial"	, &serial_testclass_t::failed_serial);
	serial_testclass->append("headtail_freelen"	, &serial_testclass_t::headtail_freelen);
	// add the testclass to the nunit_suite
	base_suite->append(serial_testclass);

/********************* delay_t	***************************************/
	// init the testclass for the delay_t
	nunit_testclass_t<delay_testclass_t> *	delay_testclass;
	delay_testclass	= nipmem_new nunit_testclass_t<delay_testclass_t>("delay"
							, nipmem_new delay_testclass_t());
	// add some test functions
	delay_testclass->append("basic_query"		, &delay_testclass_t::basic_query);
	delay_testclass->append("comparison_operator"	, &delay_testclass_t::comparison_operator);
	delay_testclass->append("arithmetic_operator"	, &delay_testclass_t::arithmetic_operator);
	delay_testclass->append("serial_consistency"	, &delay_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	base_suite->append(delay_testclass);


/********************* date_t	***************************************/
	// init the testclass for the date_t
	nunit_testclass_t<date_testclass_t> *	date_testclass;
	date_testclass	= nipmem_new nunit_testclass_t<date_testclass_t>("date"
							, nipmem_new date_testclass_t());
	// add some test functions
	date_testclass->append("canonical_string"	, &date_testclass_t::canonical_string);
	// add the testclass to the nunit_suite
	base_suite->append(date_testclass);

/********************* bitflag_t	***************************************/
	// init the testclass for the bitflag_t
	nunit_testclass_t<bitflag_testclass_t> *	bitflag_testclass;
	bitflag_testclass	= nipmem_new nunit_testclass_t<bitflag_testclass_t>("bitflag"
							, nipmem_new bitflag_testclass_t());
	// add some test functions
	bitflag_testclass->append("basic_op"		, &bitflag_testclass_t::basic_op);
	bitflag_testclass->append("boolean_op"		, &bitflag_testclass_t::boolean_op);
	bitflag_testclass->append("comparison"		, &bitflag_testclass_t::comparison);
	bitflag_testclass->append("serial_consistency"	, &bitflag_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	base_suite->append(bitflag_testclass);

/********************* string_t	***************************************/
	// init the testclass for the string_t
	nunit_testclass_t<string_testclass_t> *	string_testclass;
	string_testclass	= nipmem_new nunit_testclass_t<string_testclass_t>("string"
							, nipmem_new string_testclass_t());
	// add some test functions
	string_testclass->append("split"		, &string_testclass_t::test_split);
	string_testclass->append("replace"		, &string_testclass_t::test_replace);
	string_testclass->append("casecmp"		, &string_testclass_t::test_casecmp);
	string_testclass->append("to_upperlower"	, &string_testclass_t::test_to_upperlower);
	string_testclass->append("escape"		, &string_testclass_t::test_escape);
	string_testclass->append("strip"		, &string_testclass_t::test_strip);
	string_testclass->append("nice_string"		, &string_testclass_t::test_nice_string);
	string_testclass->append("to_uint"		, &string_testclass_t::test_to_uint);
	// add the testclass to the nunit_suite
	base_suite->append(string_testclass);


/********************* mlist_t	***************************************/
	// init the testclass for the mlist_t
	nunit_testclass_t<mlist_testclass_t> *	mlist_testclass;
	mlist_testclass	= nipmem_new nunit_testclass_t<mlist_testclass_t>("mlist"
							, nipmem_new mlist_testclass_t());
	// add some test functions
	mlist_testclass->append("general"	, &mlist_testclass_t::general);
	// add the testclass to the nunit_suite
	base_suite->append(mlist_testclass);

	// add the base_suite to the nunit_suite
	nunit_suite.append(base_suite);
}

// definition of the nunit_gsuite_t for base
NUNIT_GSUITE_DEFINITION(base_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t
NUNIT_GSUITE_INSERTER(base_nunit_gsuite, 0, base_gsuite_fct);

NEOIP_NAMESPACE_END





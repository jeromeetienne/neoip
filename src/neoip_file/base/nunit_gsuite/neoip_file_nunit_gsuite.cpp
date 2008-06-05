/*! \file
    \brief Definition of the nunit_gsuite_t for the file_t layer

*/

/* system include */
/* local include */
#include "neoip_file_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_file_size_nunit.hpp"
#include "neoip_file_path_nunit.hpp"
#include "neoip_file_path2_nunit.hpp"
#include "neoip_file_mode_nunit.hpp"
#include "neoip_file_perm_nunit.hpp"
#include "neoip_file_utils_nunit.hpp"
#include "neoip_file_dir_nunit.hpp"
#include "neoip_file_aio_nunit.hpp"
#include "neoip_file_sio_nunit.hpp"
#include "neoip_file_range_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void file_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	file_suite	= nipmem_new nunit_suite_t("file");		
	// log to debug
	KLOG_DBG("enter");
	
/********************* file_size_t	***************************************/
	// init the testclass for the file_size_t
	nunit_testclass_t<file_size_testclass_t> *	file_size_testclass;
	file_size_testclass	= nipmem_new nunit_testclass_t<file_size_testclass_t>("size", nipmem_new file_size_testclass_t());
	// add some test functions
	file_size_testclass->append("general"		, &file_size_testclass_t::general);
	file_size_testclass->append("serial_consistency", &file_size_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	file_suite->append(file_size_testclass);

/********************* file_path_t	***************************************/
	// init the testclass for the file_path_t
	nunit_testclass_t<file_path_testclass_t> *	file_path_testclass;
	file_path_testclass	= nipmem_new nunit_testclass_t<file_path_testclass_t>("path", nipmem_new file_path_testclass_t());
	// add some test functions
	file_path_testclass->append("general"		, &file_path_testclass_t::general);
	file_path_testclass->append("slash_operator"	, &file_path_testclass_t::slash_operator);
	file_path_testclass->append("name_level"	, &file_path_testclass_t::name_level);
	file_path_testclass->append("full_path"		, &file_path_testclass_t::full_path);
	file_path_testclass->append("canonisation"	, &file_path_testclass_t::canonisation);
	file_path_testclass->append("serial_consistency", &file_path_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	file_suite->append(file_path_testclass);

#if 0	// NOTE: file_path2_t is only a fork of file_path_t for easy dev
/********************* file_path2_t	***************************************/
	// init the testclass for the file_path2_t
	nunit_testclass_t<file_path2_testclass_t> *	file_path2_testclass;
	file_path2_testclass	= nipmem_new nunit_testclass_t<file_path2_testclass_t>("path2", nipmem_new file_path2_testclass_t());
	// add some test functions
	file_path2_testclass->append("general"		, &file_path2_testclass_t::general);
	file_path2_testclass->append("winpath"		, &file_path2_testclass_t::winpath);	
	file_path2_testclass->append("slash_operator"	, &file_path2_testclass_t::slash_operator);
	file_path2_testclass->append("name_level"	, &file_path2_testclass_t::name_level);
	file_path2_testclass->append("full_path2"		, &file_path2_testclass_t::full_path2);
	file_path2_testclass->append("canonisation"	, &file_path2_testclass_t::canonisation);
	file_path2_testclass->append("serial_consistency", &file_path2_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	file_suite->append(file_path2_testclass);
#endif

/********************* file_mode_t	***************************************/
	// init the testclass for the file_mode_t
	nunit_testclass_t<file_mode_testclass_t> *	file_mode_testclass;
	file_mode_testclass	= nipmem_new nunit_testclass_t<file_mode_testclass_t>("mode", nipmem_new file_mode_testclass_t());
	// add some test functions
	file_mode_testclass->append("general"	, &file_mode_testclass_t::general);
	// add the testclass to the nunit_suite
	file_suite->append(file_mode_testclass);

/********************* file_perm_t	***************************************/
	// init the testclass for the file_perm_t
	nunit_testclass_t<file_perm_testclass_t> *	file_perm_testclass;
	file_perm_testclass	= nipmem_new nunit_testclass_t<file_perm_testclass_t>("perm", nipmem_new file_perm_testclass_t());
	// add some test functions
	file_perm_testclass->append("shortcut"	, &file_perm_testclass_t::shortcut);
	file_perm_testclass->append("to_octal"	, &file_perm_testclass_t::to_octal);
	file_perm_testclass->append("from_octal", &file_perm_testclass_t::from_octal);
	// add the testclass to the nunit_suite
	file_suite->append(file_perm_testclass);
	
/********************* file_utils_t	***************************************/
	// init the testclass for the file_utils_t
	nunit_testclass_t<file_utils_testclass_t> *	file_utils_testclass;
	file_utils_testclass	= nipmem_new nunit_testclass_t<file_utils_testclass_t>("utils", nipmem_new file_utils_testclass_t());
	// add some test functions
	file_utils_testclass->append("get_current_dir"	, &file_utils_testclass_t::get_current_dir);
	file_utils_testclass->append("get_home_dir"	, &file_utils_testclass_t::get_home_dir);
	file_utils_testclass->append("create_remove_dir_no_recursion", &file_utils_testclass_t::create_remove_dir_no_recursion);
	file_utils_testclass->append("create_remove_dir_do_recursion", &file_utils_testclass_t::create_remove_dir_do_recursion);
	file_utils_testclass->append("umask"		, &file_utils_testclass_t::umask);
	// add the testclass to the nunit_suite
	file_suite->append(file_utils_testclass);
	
/********************* file_dir_t	***************************************/
	// init the testclass for the file_dir_t
	nunit_testclass_t<file_dir_testclass_t> *	file_dir_testclass;
	file_dir_testclass	= nipmem_new nunit_testclass_t<file_dir_testclass_t>("dir", nipmem_new file_dir_testclass_t());
	// add some test functions
	file_dir_testclass->append("general"	, &file_dir_testclass_t::general);
	// add the testclass to the nunit_suite
	file_suite->append(file_dir_testclass);
	
/********************* file_aio_t	***************************************/
	// init the testclass for the file_aio_t
	nunit_testclass_t<file_aio_testclass_t> *	file_aio_testclass;
	file_aio_testclass	= nipmem_new nunit_testclass_t<file_aio_testclass_t>("aio", nipmem_new file_aio_testclass_t());
	// add some test functions
	file_aio_testclass->append("awrite"	, &file_aio_testclass_t::awrite);
	file_aio_testclass->append("aread"	, &file_aio_testclass_t::aread);
	// add the testclass to the nunit_suite
	file_suite->append(file_aio_testclass);
	
/********************* file_sio_t	***************************************/
	// init the testclass for the file_sio_t
	nunit_testclass_t<file_sio_testclass_t> *	file_sio_testclass;
	file_sio_testclass	= nipmem_new nunit_testclass_t<file_sio_testclass_t>("sio", nipmem_new file_sio_testclass_t());
	// add some test functions
	file_sio_testclass->append("write"	, &file_sio_testclass_t::write);
	file_sio_testclass->append("read"	, &file_sio_testclass_t::read);
	// add the testclass to the nunit_suite
	file_suite->append(file_sio_testclass);
	
/********************* file_range_t	***************************************/
	// init the testclass for the file_range_t
	nunit_testclass_t<file_range_testclass_t> *	file_range_testclass;
	file_range_testclass	= nipmem_new nunit_testclass_t<file_range_testclass_t>("range", nipmem_new file_range_testclass_t());
	// add some test functions
	file_range_testclass->append("general"		, &file_range_testclass_t::general);
	file_range_testclass->append("range_op"		, &file_range_testclass_t::range_op);
	file_range_testclass->append("comparison"	, &file_range_testclass_t::comparison);
	// add the testclass to the nunit_suite
	file_suite->append(file_range_testclass);
	// add the file_suite to the nunit_suite
	nunit_suite.append(file_suite);	
}

// definition of the nunit_gsuite_t for file
NUNIT_GSUITE_DEFINITION(file_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(file_nunit_gsuite, 0, file_gsuite_fct);

NEOIP_NAMESPACE_END





/*! \file
    \brief Definition of the unit test for the \ref file_utils_t

*/

/* system include */
/* local include */
#include "neoip_file_utils_nunit.hpp"
#include "neoip_file_utils.hpp"
#include "neoip_file_perm.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_utils_testclass_t::get_current_dir(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("file_utils_t::get_current_dir()=" << file_utils_t::get_current_dir().to_os_path_string());
	
	NUNIT_ASSERT( file_utils_t::get_current_dir() == "/home/jerome/workspace/yavipin/src" );
	// report no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_utils_testclass_t::get_home_dir(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
#ifndef	_WIN32	// file_path_t::fullpath_t use the current dir.
		// which is different under native linux and wine
	std::string	expected_result	= "/home/jerome";
#else
	std::string	expected_result	= "z:\\home\\jerome";
	// NOTE: the home directory is a pain to get under window.. - see file_oswarp_t
	// - apparently wine do not export HOME or HOMEPATH..
	expected_result	= "c:\\windows\\system32";	
#endif
	// log to debug
	KLOG_ERR("file_utils_t::get_home_dir()=" << file_utils_t::get_home_dir());
	KLOG_ERR("expected_result=" << expected_result);
	
	file_path_t	home_dir	= file_utils_t::get_home_dir();
	NUNIT_ASSERT( file_utils_t::get_home_dir() == expected_result );
	// report no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_utils_testclass_t::create_remove_dir_no_recursion(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_path_t	dir_path	= file_utils_t::get_temp_path("/tmp/neoip_file_utils_nunit");
	file_err_t	file_err;
	// log to debug
	KLOG_DBG("enter dir_path=" << dir_path);
	// create the directory
	file_err	= file_utils_t::create_directory(dir_path, file_utils_t::NO_RECURSION);
	KLOG_ERR("file_err=" << file_err);
	NUNIT_ASSERT( file_err.succeed() );
	// test if the file exists and is a directory
	NUNIT_ASSERT( file_stat_t(dir_path).is_dir() );
	// remove the directory
	file_err	= file_utils_t::remove_directory(dir_path, file_utils_t::NO_RECURSION);
	KLOG_ERR("file_err=" << file_err);
	NUNIT_ASSERT( file_err.succeed() );
	// test if the file no more exist
	NUNIT_ASSERT( file_stat_t(dir_path).is_null() );
	// report no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_utils_testclass_t::create_remove_dir_do_recursion(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_path_t	dir_path	= file_utils_t::get_temp_path("/tmp/neoip_file_utils_nunit");
	file_path_t	subdir_path	= dir_path / "sampledir1" / "sampledir2";
	file_err_t	file_err;
	// log to debug
	KLOG_DBG("enter dir_path=" << dir_path.to_os_path_string());
	// create the directory
	file_err	= file_utils_t::create_directory(subdir_path, file_utils_t::DO_RECURSION);
	KLOG_ERR("file_err=" << file_err);
	NUNIT_ASSERT( file_err.succeed() );
	// test if the file exists and is a directory
	NUNIT_ASSERT( file_stat_t(subdir_path).is_dir() );
	// create a file in the subdir_path
	{file_sio_t	file_sio;
	file_err	= file_sio.start(subdir_path / "samplefile", file_mode_t::READ | file_mode_t::CREATE);
	NUNIT_ASSERT( file_err.succeed() );
	}
	// NOTE: the file NEEDS to be closed on unlink on WIN32
	// - on linux this is not a requirement
	// test if the file exists
	NUNIT_ASSERT( file_stat_t(subdir_path / "samplefile").is_null() == false );
	// remove the directory
	file_err	= file_utils_t::remove_directory(dir_path, file_utils_t::DO_RECURSION);
	NUNIT_ASSERT( file_err.succeed() );
	// test if the file no more exist
	NUNIT_ASSERT( file_stat_t(dir_path).is_null() );
	// report no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_utils_testclass_t::umask(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_perm_t	file_perm;
	file_err_t	file_err;
	// get the original file_perm_t
	file_perm	= file_utils_t::umask();
	// set the umask to a new value
	file_err	= file_utils_t::umask(file_perm_t::OTH_RWX);
	NUNIT_ASSERT( file_err.succeed() );
	// check that the new umask is the expected one
	NUNIT_ASSERT( file_utils_t::umask() == file_perm_t::OTH_RWX );	
	// restore the original umask
	file_err	= file_utils_t::umask(file_perm);
	NUNIT_ASSERT( file_err.succeed() );
	// check that the new umask is the expected one
	NUNIT_ASSERT( file_utils_t::umask() == file_perm );	
	// report no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the unit test for the \ref file_dir_t

*/

/* system include */
/* local include */
#include "neoip_file_dir_nunit.hpp"
#include "neoip_file_dir.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_dir_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_dir_t	file_dir;
	file_err_t	file_err;
	
	file_err	= file_dir.open("/tmp");
	NUNIT_ASSERT( file_err.succeed() );
	
	// sort the result 
	file_dir.sort();

	// display the directories
	for( size_t i = 0; i < file_dir.size(); i++ ){
		// display this directory
		KLOG_ERR( file_dir[i] );
	}
	// report no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END


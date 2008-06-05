/*! \file
    \brief Definition of the unit test for the \ref file_mode_t

*/

/* system include */
/* local include */
#include "neoip_file_mode_nunit.hpp"
#include "neoip_file_mode.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_mode_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_mode_t	file_mode	= file_mode_t::READ;
	
	NUNIT_ASSERT( file_mode	== file_mode_t::READ );
	NUNIT_ASSERT( file_mode.include(file_mode_t::READ) );
	
	file_mode	+= file_mode_t::WRITE;
	NUNIT_ASSERT( file_mode.include(file_mode_t::READ) );
	NUNIT_ASSERT( file_mode.include(file_mode_t::WRITE) );
	
	file_mode	= file_mode_t::RW;	
	NUNIT_ASSERT( file_mode.include(file_mode_t::READ) );
	NUNIT_ASSERT( file_mode.include(file_mode_t::WRITE) );
	
	// report no error
	return NUNIT_RES_OK;
}



NEOIP_NAMESPACE_END


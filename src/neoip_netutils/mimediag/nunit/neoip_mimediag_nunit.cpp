/*! \file
    \brief unit test for the mimediag_t
*/

/* system include */
/* local include */
#include "neoip_mimediag_nunit.hpp"
#include "neoip_mimediag.hpp"
#include "neoip_file_path.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the mimediag_t
 */
nunit_res_t	mimediag_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// log to debug
	KLOG_DBG("enter");
	
	// test a non existing mimetype with a default
	NUNIT_ASSERT( mimediag_t::from_file_ext("groubaaaa", "nothing") == "nothing" );

	// test a non existing mimetype with no default
	NUNIT_ASSERT( mimediag_t::from_file_ext("groubaaaa").empty() );
	
	// test an existing mimetype from a file extention
	NUNIT_ASSERT( mimediag_t::from_file_ext("ogg") == "application/ogg" );

	// test an existing mimetype from a file path
	NUNIT_ASSERT( mimediag_t::from_file_path("mywondermusic.ogg") == "application/ogg" );

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

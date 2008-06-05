/*! \file
    \brief Definition of the unit test for the \ref http_reqhd_t

*/

/* system include */
/* local include */
#include "neoip_http_reqhd_nunit.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_file_range.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	http_reqhd_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// build a http_reqhd_t
	http_reqhd_t	http_reqhd;
	http_reqhd.method("GET").uri("http://0.0.0.0/blabla").version("HTTP/1.1");
	http_reqhd.header_db().append("foo", "bar");
	
	// cycle out thru http encoding
	http_reqhd_t	http_reqhd2	= http_reqhd_t::from_http(http_reqhd.to_http());

	// test the serial consistency
	NUNIT_ASSERT( http_reqhd == http_reqhd2 );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief Test function
 */
nunit_res_t	http_reqhd_testclass_t::header_range(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	http_reqhd_t	http_reqhd;
	// log to debug
	KLOG_DBG("enter");

	// test parsing of a fully qualified range
	http_reqhd	= http_reqhd_t();
	http_reqhd.header_db().append("Range", "bytes=1-3");
	NUNIT_ASSERT( http_reqhd.range() == file_range_t(1, 3) );

	// test parsing of an open ended range
	http_reqhd	= http_reqhd_t();
	http_reqhd.header_db().append("Range", "bytes=1-");
	NUNIT_ASSERT( http_reqhd.range() == file_range_t(1, file_size_t::MAX) );

	// test parsing of a set of range - it is unsupported aka return null
	http_reqhd	= http_reqhd_t();
	http_reqhd.header_db().append("Range", "bytes=1-3, 5-7");
	NUNIT_ASSERT( http_reqhd.range() == file_range_t() );

	// test building a fully qualified range
	NUNIT_ASSERT(http_reqhd_t().range(file_range_t(1,3)).header_db()
					.get_first_value("Range")=="bytes=1-3");
	// test building an open ended range
	NUNIT_ASSERT(http_reqhd_t().range(file_range_t(1,file_size_t::MAX)).header_db()
					.get_first_value("Range")=="bytes=1-");

	// test serial consistancy of an fully qualified range
	NUNIT_ASSERT(http_reqhd_t().range(file_range_t(1,3)).range() == file_range_t(1,3));

	// test serial consistancy of an open ended range
	NUNIT_ASSERT(http_reqhd_t().range(file_range_t(1,file_size_t::MAX)).range() == file_range_t(1,file_size_t::MAX));

	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END


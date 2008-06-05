/*! \file
    \brief Definition of the unit test for the \ref http_rephd_t

*/

/* system include */
/* local include */
#include "neoip_http_rephd_nunit.hpp"
#include "neoip_http_rephd.hpp"
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
nunit_res_t	http_rephd_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");	

	// build a http_rephd_t
	http_rephd_t	http_rephd;
	http_rephd.version("HTTP/1.1").status_code(200).reason_phrase("all is ok");
	http_rephd.header_db().append("foo", "bar");
	
	// cycle out thru http encoding
	http_rephd_t	http_rephd2	= http_rephd_t::from_http(http_rephd.to_http());

	// test the serial consistency
	NUNIT_ASSERT( http_rephd == http_rephd2 );
	
	// return no error
	return NUNIT_RES_OK;
}

/** \brief Test function
 */
nunit_res_t	http_rephd_testclass_t::header_content_length(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	http_rephd_t	http_rephd;
	// log to debug
	KLOG_DBG("enter");

	// test parsing of a Content-Length
	http_rephd	= http_rephd_t();
	http_rephd.header_db().append("Content-Length", "4242");
	NUNIT_ASSERT( http_rephd.content_length() == 4242 );

	// test building a Content-Length
	NUNIT_ASSERT(http_rephd_t().content_length(4242).header_db()
					.get_first_value("Content-Length")== "4242");

	// test serial consistancy
	NUNIT_ASSERT(http_rephd_t().content_length(4242).content_length() == 4242);
	
	// return no error
	return NUNIT_RES_OK;
}


/** \brief Test function
 */
nunit_res_t	http_rephd_testclass_t::header_content_range(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	http_rephd_t	http_rephd;
	file_size_t	instance_len;
	// log to debug
	KLOG_DBG("enter");
	
	// test parsing of a fully qualified Content-Range
	http_rephd	= http_rephd_t();
	http_rephd.header_db().append("Content-Range", "bytes 1-3/4242");
	NUNIT_ASSERT( http_rephd.content_range(&instance_len) == file_range_t(1, 3) );	
	NUNIT_ASSERT( instance_len == 4242 );

	// test parsing of a Content-Range with undetermined range and undetermined instance len
	http_rephd	= http_rephd_t();
	http_rephd.header_db().append("Content-Range", "bytes */*");
	NUNIT_ASSERT( http_rephd.content_range(&instance_len) == file_range_t() );	
	NUNIT_ASSERT( instance_len == file_size_t() );

	// test parsing of a Content-Range with undetermined range and determined instance len
	http_rephd	= http_rephd_t();
	http_rephd.header_db().append("Content-Range", "bytes */4242");
	NUNIT_ASSERT( http_rephd.content_range(&instance_len) == file_range_t() );	
	NUNIT_ASSERT( instance_len == 4242 );

	// test parsing of a Content-Range with determined range and undetermined instance len
	http_rephd	= http_rephd_t();
	http_rephd.header_db().append("Content-Range", "bytes 1-3/*");
	NUNIT_ASSERT( http_rephd.content_range(&instance_len) == file_range_t(1, 3) );	
	NUNIT_ASSERT( instance_len == file_size_t() );
	
	// test building a fully qualified one
	NUNIT_ASSERT(http_rephd_t().content_range(file_range_t(1,3), 4242).header_db()
					.get_first_value("Content-Range")=="bytes 1-3/4242");
	// test building with no instance length
	NUNIT_ASSERT(http_rephd_t().content_range(file_range_t(1,3), file_size_t()).header_db()
					.get_first_value("Content-Range")=="bytes 1-3/*");	
	// test building with no file_range and an instance length
	NUNIT_ASSERT(http_rephd_t().content_range(file_range_t(), 4242).header_db()
					.get_first_value("Content-Range")=="bytes */4242");	
	// return no error
	return NUNIT_RES_OK;
}



NEOIP_NAMESPACE_END


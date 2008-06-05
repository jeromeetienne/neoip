/*! \file
    \brief unit test for the http_nested_uri_t unit test

*/

/* system include */
/* local include */
#include "neoip_http_nested_uri_nunit.hpp"
#include "neoip_http_nested_uri.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	http_nested_uri_testclass_t::is_valid_outter(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	/*************** test is_valid_outter	*******************************/
	NUNIT_ASSERT(  http_nested_uri_t::is_valid_outter("http://example.com") );
	NUNIT_ASSERT(  http_nested_uri_t::is_valid_outter("http://example.com/http") );
	NUNIT_ASSERT(  http_nested_uri_t::is_valid_outter("http://example.com/http:") );
	NUNIT_ASSERT( !http_nested_uri_t::is_valid_outter("http://example.com/http://bla") );
	NUNIT_ASSERT( !http_nested_uri_t::is_valid_outter("http://example.com/*pathname") );
	NUNIT_ASSERT( !http_nested_uri_t::is_valid_outter("http://example.com?slo*ta=bli") );
	NUNIT_ASSERT( !http_nested_uri_t::is_valid_outter("http://example.com?slo*ta=bloi") );
	NUNIT_ASSERT( !http_nested_uri_t::is_valid_outter("http://example.com#my_anchor") );
	NUNIT_ASSERT(  http_nested_uri_t::is_valid_outter("http://example.com?slota=/http:/") );
	NUNIT_ASSERT(  http_nested_uri_t::is_valid_outter("http://example.com?slota=/http%3a/") );

	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	http_nested_uri_testclass_t::nested_consistency1(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	http_uri_t	outter_uri	= "http://static.nunet.net/slota/sloti/bla?xmit_maxrate=2k";
	http_uri_t	inner_uri	= "http://192.168.1.3/~jerome";
	http_uri_t	nested_uri	= "http://static.nunet.net/slota/sloti/bla/*xmit_maxrate*2k/http://192.168.1.3/~jerome";
	NUNIT_ASSERT( http_nested_uri_t::is_valid_outter(outter_uri ) );
	NUNIT_ASSERT( http_nested_uri_t::build(outter_uri, inner_uri) == nested_uri);
	NUNIT_ASSERT( http_nested_uri_t::is_valid_nested(nested_uri) );
	NUNIT_ASSERT( http_nested_uri_t::parse_outter(nested_uri) == outter_uri);
	NUNIT_ASSERT( http_nested_uri_t::parse_inner(nested_uri) == inner_uri);
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	http_nested_uri_testclass_t::nested_consistency2(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// test a general case
	http_uri_t	inner_uri	= "http://192.168.1.3:8080/~jerome/IB%20M.avi?slota=bli";
	http_uri_t	outter_uri	= "http://static.nunet.net:69/bl%20ou?foo=bar&file_size=125";
	http_uri_t	nested_uri	= "http://static.nunet.net:69/bl%20ou/*foo*bar/*file_size*125/http://192.168.1.3:8080/~jerome/IB%20M.avi?slota=bli";
	NUNIT_ASSERT( http_nested_uri_t::is_valid_outter(outter_uri ) );
	NUNIT_ASSERT( http_nested_uri_t::build(outter_uri, inner_uri) == nested_uri);
	NUNIT_ASSERT( http_nested_uri_t::is_valid_nested(nested_uri) );
	NUNIT_ASSERT( http_nested_uri_t::parse_outter(nested_uri) == outter_uri);
	NUNIT_ASSERT( http_nested_uri_t::parse_inner(nested_uri) == inner_uri);

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

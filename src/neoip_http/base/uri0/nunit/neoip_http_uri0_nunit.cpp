/*! \file
    \brief unit test for the http_uri_t unit test

*/

/* system include */
/* local include */
#include "neoip_http_uri_nunit.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	http_uri_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// some basic testing of the http_uri_t class
	NUNIT_ASSERT( http_uri_t("slota").is_null() );
	NUNIT_ASSERT( http_uri_t("http://off.net/~jme").host() == "off.net" );
	NUNIT_ASSERT( http_uri_t("http://off.net:8080/~jme").port() == 8080 );
	NUNIT_ASSERT( http_uri_t("http://off.net/~jme").path() == "/~jme" );
	NUNIT_ASSERT( http_uri_t("http://off.net/~jme").anchor().empty() );
	NUNIT_ASSERT( http_uri_t("http://off.net/~jme#anchooor").anchor() == "anchooor" );
	NUNIT_ASSERT( http_uri_t("http://off.net/~jme").to_string() == "http://off.net/~jme");
	NUNIT_ASSERT( http_uri_t("http://off.net/~jme/my%20path").to_string() == "http://off.net/~jme/my%20path");
	
	NUNIT_ASSERT( http_uri_t("http://off.net") == http_uri_t("http://OFF.Net"));
	
	// return no error
	return NUNIT_RES_OK;
}


/** \brief test the serial consistency
 */
nunit_res_t	http_uri_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	http_uri_t	http_uri_toserial = "http://off.net/~jme?name1=alice#anchooor";
	http_uri_t	http_uri_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << http_uri_toserial;
	serial >> http_uri_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( http_uri_toserial == http_uri_unserial );

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

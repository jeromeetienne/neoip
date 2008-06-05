/*! \file
    \brief Definition of the unit test for the \ref xmlrpc_t

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_build_nunit.hpp"
#include "neoip_xmlrpc_build.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	xmlrpc_build_testclass_t::example_from_spec(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

// test the CALL as in the http://www.xmlrpc.com/spec example
{
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("examples.getStateName");
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG;
	xmlrpc_build		<< int32_t(41);
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build << xmlrpc_build_t::CALL_END;

	datum_t	expected_datum	= datum_t("<?xml version=\"1.0\"?>\n<methodCall><methodName>examples.getStateName</methodName><params><param><value><int>41</int></value></param></params></methodCall>\n");
	KLOG_DBG("xmlrpc_build.to_datum()=" << xmlrpc_build.to_stdstring());
	KLOG_DBG("expected_datum         =" << expected_datum.to_stdstring());
	NUNIT_ASSERT( xmlrpc_build.to_datum() == expected_datum );
}


// test the RESP without fault as in the http://www.xmlrpc.com/spec example
{
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::RESP_BEG;
	xmlrpc_build	<< std::string("South dakota");
	xmlrpc_build << xmlrpc_build_t::RESP_END;

	datum_t	expected_datum	= datum_t("<?xml version=\"1.0\"?>\n<methodResponse><params><param><value><string>South dakota</string></value></param></params></methodResponse>\n");
	KLOG_DBG("xmlrpc_build.to_datum()=" << xmlrpc_build.to_stdstring());
	KLOG_DBG("expected_datum         =" << expected_datum.to_stdstring());
	NUNIT_ASSERT( xmlrpc_build.to_datum() == expected_datum );
}

	
// test the RESP with fault as in the http://www.xmlrpc.com/spec example
{
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::FAULT(4, "Too many parameters");

	datum_t	expected_datum	= datum_t("<?xml version=\"1.0\"?>\n<methodResponse><fault><value><struct><member><name>faultCode</name><value><int>4</int></value></member><member><name>faultString</name><value><string>Too many parameters</string></value></member></struct></value></fault></methodResponse>\n");
	KLOG_DBG("xmlrpc_build.to_datum()=" << xmlrpc_build.to_stdstring());
	KLOG_DBG("expected_datum         =" << expected_datum.to_stdstring());
	NUNIT_ASSERT( xmlrpc_build.to_datum() == expected_datum );
}
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the unit test for the \ref xmlrpc_t

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_parse_nunit.hpp"
#include "neoip_xmlrpc_parse.hpp"
#include "neoip_xmlrpc_build.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_xml.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	xmlrpc_parse_testclass_t::example_from_spec(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

// test the CALL as in the http://www.xmlrpc.com/spec example
{
	datum_t		xmlenc_datum	= datum_t("<?xml version=\"1.0\"?>\n<methodCall><methodName>examples.getStateName</methodName><params><param><value><int>41</int></value></param></params></methodCall>\n");
	xmlrpc_parse_t	xmlrpc_parse;
	// set the document for xmlrpc_parse_t
	xmlrpc_parse.set_document(xmlenc_datum);
	NUNIT_ASSERT( !xmlrpc_parse.is_null() );
	// declare the variable to read from the xmlrpc
	std::string	method_name;
	int32_t		value;
	try {
		xmlrpc_parse >> xmlrpc_parse_t::CALL_BEG(method_name);
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEG;
		xmlrpc_parse		>> value;
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::CALL_END;
	} catch(xml_except_t &e){
		KLOG_ERR("xml_except_t=" << e.what());
		NUNIT_ASSERT(0);
	}
	// check the parameters have the expected values
	NUNIT_ASSERT( method_name == "examples.getStateName" );	
	NUNIT_ASSERT( value == 41 );	
}

// test the RESP without fault as in the http://www.xmlrpc.com/spec example
{
	datum_t		xmlenc_datum	= datum_t("<?xml version=\"1.0\"?>\n<methodResponse><params><param><value><string>South dakota</string></value></param></params></methodResponse>\n");
	xmlrpc_parse_t	xmlrpc_parse;
	// set the document for xmlrpc_parse_t
	xmlrpc_parse.set_document(xmlenc_datum);
	NUNIT_ASSERT( !xmlrpc_parse.is_null() );
	// declare the variable to read from the xmlrpc
	std::string	param_str;
	try {
		xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
		xmlrpc_parse	>> param_str;
		xmlrpc_parse >> xmlrpc_parse_t::RESP_END;
	} catch(xml_except_t &e) {
		KLOG_ERR("xml_except_t=" << e.what());
		NUNIT_ASSERT(0);
	}
	// check the parameters have the expected values
	NUNIT_ASSERT( param_str == "South dakota" );	
}

// test the RESP with fault as in the http://www.xmlrpc.com/spec example
{
	datum_t		xmlenc_datum	= datum_t("<?xml version=\"1.0\"?>\n<methodResponse><fault><value><struct><member><name>faultCode</name><value><int>4</int></value></member><member><name>faultString</name><value><string>Too many parameters</string></value></member></struct></value></fault></methodResponse>\n");
	xmlrpc_parse_t	xmlrpc_parse;
	// set the document for xmlrpc_parse_t
	xmlrpc_parse.set_document(xmlenc_datum);
	NUNIT_ASSERT( !xmlrpc_parse.is_null() );
	// declare the variable to read from the xmlrpc
	int32_t		fault_code;
	std::string	fault_string;
	try {
		NUNIT_ASSERT( xmlrpc_parse.is_fault_resp() );
		xmlrpc_parse >> xmlrpc_parse_t::FAULT(fault_code, fault_string);
	} catch(xml_except_t &e) {
		KLOG_ERR("xml_except_t=" << e.what());
		NUNIT_ASSERT(0);
	};
	// check the parameters have the expected values
	NUNIT_ASSERT( fault_code == 4 );	
	NUNIT_ASSERT( fault_string == "Too many parameters" );	
}

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	xmlrpc_parse_testclass_t::to_json(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

// TODO write a real nunit


	// build a xmlrpc
	xmlrpc_build_t	xmlrpc_build;
#if 0
	xmlrpc_build	<< double(41);
#endif
#if 0
	xmlrpc_build	<< xmlrpc_build_t::ARRAY_BEG;
	xmlrpc_build	<< int32_t(42);
	xmlrpc_build	<< std::string("bonjour");
	xmlrpc_build	<< false;
	xmlrpc_build	<< xmlrpc_build_t::ARRAY_END;
#endif
#if 1
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("full name");
	xmlrpc_build		<< xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_BEG("first name");
	xmlrpc_build				<< std::string("john");
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_BEG("last name");
	xmlrpc_build				<< std::string("smith");
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_BEG("blou_arr");
	xmlrpc_build	<< xmlrpc_build_t::ARRAY_BEG;
	xmlrpc_build	<< int32_t(42);
	xmlrpc_build	<< std::string("bonjour\nles\"amis");
	xmlrpc_build	<< false;
	xmlrpc_build	<< xmlrpc_build_t::ARRAY_END;
	xmlrpc_build			<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build		<< xmlrpc_build_t::STRUCT_END;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
#endif
#if 0
	xmlrpc_build	<< xmlrpc_build_t::ARRAY_BEG;
	xmlrpc_build	<< std::string("bon \" jour");
	xmlrpc_build	<< std::string("bon \\ jour");
	xmlrpc_build	<< std::string("bon / jour");
	xmlrpc_build	<< std::string("bon \b jour");	// TODO some issue with \b
	xmlrpc_build	<< std::string("bon \f jour");	// TODO some issue with \f...
							// likely comes from the xml lib itself
	xmlrpc_build	<< std::string("bon \n jour");
	xmlrpc_build	<< std::string("bon \r jour");
	xmlrpc_build	<< std::string("bon \t jour");
	xmlrpc_build	<< xmlrpc_build_t::ARRAY_END;
#endif

	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_build.to_datum());
	NUNIT_ASSERT( !xmlrpc_parse.is_null() );
	

	std::ostringstream	oss_json;
	try {
		xmlrpc_parse	>> xmlrpc_parse_t::VALUE_TO_JSON(oss_json);
	} catch(xml_except_t &e){
		KLOG_ERR("xml_except_t=" << e.what());
		NUNIT_ASSERT(0);
	}
	KLOG_ERR("json=" << oss_json.str());
	
	// return no error
	return NUNIT_RES_OK;
}
NEOIP_NAMESPACE_END


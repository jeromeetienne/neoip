/*! \file
    \brief Definition of the unit test for the \ref xmlrpc_t

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_consistency_nunit.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	xmlrpc_consistency_testclass_t::call_example_from_spec(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
// build the xmlrpc
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("examples.getStateName");
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG;
	xmlrpc_build		<< int32_t(41);
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build << xmlrpc_build_t::CALL_END;


// parse the xmlrpc
	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_build.to_datum());
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
nunit_res_t	xmlrpc_consistency_testclass_t::resp_example_from_spec(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
// build the xmlrpc
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::RESP_BEG;
	xmlrpc_build	<< std::string("South dakota");
	xmlrpc_build << xmlrpc_build_t::RESP_END;

// parse the xmlrpc
	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_build.to_datum());
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
nunit_res_t	xmlrpc_consistency_testclass_t::fault_example_from_spec(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
// build the xmlrpc
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::FAULT(4, "Too many parameters");

// parse the xmlrpc
	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_build.to_datum());
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
nunit_res_t	xmlrpc_consistency_testclass_t::call_with_empty_array(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
// build the xmlrpc
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("examples.getStateName");
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG;
	xmlrpc_build		<< xmlrpc_build_t::ARRAY_BEG;
	xmlrpc_build		<< xmlrpc_build_t::ARRAY_END;
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG;
	xmlrpc_build		<< double(0.5);
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;	
	xmlrpc_build << xmlrpc_build_t::CALL_END;


// parse the xmlrpc
	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_build.to_datum());
	NUNIT_ASSERT( !xmlrpc_parse.is_null() );
	// declare the variable to read from the xmlrpc
	std::string	method_name;
	double		double_val;
	try {
		xmlrpc_parse >> xmlrpc_parse_t::CALL_BEG(method_name);
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEG;
		xmlrpc_parse		>> xmlrpc_parse_t::ARRAY_BEG;
		NUNIT_ASSERT( !xmlrpc_parse.has_more_sibling() );
		xmlrpc_parse		>> xmlrpc_parse_t::ARRAY_END;
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEG;
		xmlrpc_parse		>> double_val;
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::CALL_END;
	} catch(xml_except_t &e){
		KLOG_ERR("xml_except_t=" << e.what());
		NUNIT_ASSERT(0);
	}
	// check the parameters have the expected values
	NUNIT_ASSERT( method_name == "examples.getStateName" );	
	NUNIT_ASSERT( double_val == 0.5 );
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
nunit_res_t	xmlrpc_consistency_testclass_t::call_with_no_param(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
// build the xmlrpc
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("examples.getStateName");
	xmlrpc_build << xmlrpc_build_t::CALL_END;


// parse the xmlrpc
	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_build.to_datum());
	NUNIT_ASSERT( !xmlrpc_parse.is_null() );
	// declare the variable to read from the xmlrpc
	std::string	method_name;
	try {
		xmlrpc_parse >> xmlrpc_parse_t::CALL_BEG(method_name);
		NUNIT_ASSERT( !xmlrpc_parse.has_more_sibling() );
		xmlrpc_parse >> xmlrpc_parse_t::CALL_END;
	} catch(xml_except_t &e){
		KLOG_ERR("xml_except_t=" << e.what());
		NUNIT_ASSERT(0);
	}
	// check the parameters have the expected values
	NUNIT_ASSERT( method_name == "examples.getStateName" );	
	
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
nunit_res_t	xmlrpc_consistency_testclass_t::resp_with_empty_struct(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
// build the xmlrpc
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::RESP_BEG;
	xmlrpc_build	<< xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build	<< xmlrpc_build_t::STRUCT_END;
	xmlrpc_build << xmlrpc_build_t::RESP_END;

// parse the xmlrpc
	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_build.to_datum());
	NUNIT_ASSERT( !xmlrpc_parse.is_null() );
	try {
		xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
		xmlrpc_parse	>> xmlrpc_parse_t::STRUCT_BEG;
		NUNIT_ASSERT( !xmlrpc_parse.has_member("slota") );
		xmlrpc_parse	>> xmlrpc_parse_t::STRUCT_END;
		xmlrpc_parse >> xmlrpc_parse_t::RESP_END;
	} catch(xml_except_t &e) {
		KLOG_ERR("xml_except_t=" << e.what());
		NUNIT_ASSERT(0);
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
nunit_res_t	xmlrpc_consistency_testclass_t::resp_with_complex_struct(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
// build the xmlrpc
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::RESP_BEG;
	xmlrpc_build	<< xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build		<< xmlrpc_build_t::MEMBER_BEG("empty array");
	xmlrpc_build			<< xmlrpc_build_t::ARRAY_BEG;
	xmlrpc_build			<< xmlrpc_build_t::ARRAY_END;
	xmlrpc_build		<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build		<< xmlrpc_build_t::MEMBER_BEG("full name");
	xmlrpc_build			<< xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build				<< xmlrpc_build_t::MEMBER_BEG("first name");
	xmlrpc_build					<< std::string("john");
	xmlrpc_build				<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build				<< xmlrpc_build_t::MEMBER_BEG("last name");
	xmlrpc_build					<< std::string("smith");
	xmlrpc_build				<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build			<< xmlrpc_build_t::STRUCT_END;
	xmlrpc_build		<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build	<< xmlrpc_build_t::STRUCT_END;
	xmlrpc_build << xmlrpc_build_t::RESP_END;

// parse the xmlrpc
	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(xmlrpc_build.to_datum());
	NUNIT_ASSERT( !xmlrpc_parse.is_null() );
	// declare the variable to read from the xmlrpc
	std::string	firstname_str;
	std::string	lastname_str;
	try {
		xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
		NUNIT_ASSERT( xmlrpc_parse.value_type() == "struct" );
		xmlrpc_parse	>> xmlrpc_parse_t::STRUCT_BEG;
		NUNIT_ASSERT		( !xmlrpc_parse.has_member("slota") );
		NUNIT_ASSERT		( xmlrpc_parse.has_member("full name") );
		NUNIT_ASSERT		( xmlrpc_parse.has_member("empty array") );
		xmlrpc_parse		>> xmlrpc_parse_t::MEMBER_BEG("full name");	
		xmlrpc_parse			>> xmlrpc_parse_t::STRUCT_BEG;
		NUNIT_ASSERT				(!xmlrpc_parse.has_member("slota") );
		NUNIT_ASSERT				( xmlrpc_parse.has_member("first name") );
		NUNIT_ASSERT				( xmlrpc_parse.has_member("last name") );
		xmlrpc_parse				>> xmlrpc_parse_t::MEMBER_BEG("last name");	
		xmlrpc_parse					>> lastname_str;
		xmlrpc_parse				>> xmlrpc_parse_t::MEMBER_END;
		xmlrpc_parse				>> xmlrpc_parse_t::MEMBER_BEG("first name");	
		xmlrpc_parse					>> firstname_str;
		xmlrpc_parse				>> xmlrpc_parse_t::MEMBER_END;		
		xmlrpc_parse			>> xmlrpc_parse_t::STRUCT_END;		
		xmlrpc_parse		>> xmlrpc_parse_t::MEMBER_END;
		xmlrpc_parse		>> xmlrpc_parse_t::MEMBER_BEG("empty array");
		xmlrpc_parse			>> xmlrpc_parse_t::ARRAY_BEG;
		NUNIT_ASSERT				( !xmlrpc_parse.has_more_sibling() );
		xmlrpc_parse			>> xmlrpc_parse_t::ARRAY_END;
		xmlrpc_parse		>> xmlrpc_parse_t::MEMBER_END;		
		xmlrpc_parse	>> xmlrpc_parse_t::STRUCT_END;
		xmlrpc_parse >> xmlrpc_parse_t::RESP_END;
	} catch(xml_except_t &e) {
		KLOG_ERR("xml_except_t=" << e.what());
		NUNIT_ASSERT(0);
	}
	// check the parameters have the expected values
	NUNIT_ASSERT( firstname_str == "john" );
	NUNIT_ASSERT( lastname_str == "smith" );
	
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
nunit_res_t	xmlrpc_consistency_testclass_t::invalid_xml(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// set a invalid xml in the datum
	datum_t		xmlenc_datum	= datum_t("<?xml version=\"1.0\"?>\n<int>5</iiiint>\n");
	xmlrpc_parse_t	xmlrpc_parse;
	// set the document for xmlrpc_parse_t
	xmlrpc_parse.set_document(xmlenc_datum);
	// the xmlrpc_parse_t MUST be null
	NUNIT_ASSERT( xmlrpc_parse.is_null() );
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END


/*! \file
    \brief unit test for the ipport_aview_t unit test

*/

/* system include */
/* local include */
#include "neoip_ipport_aview_nunit.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the comparison operator
 */
nunit_res_t	ipport_aview_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// test that the default ctor produce a null object
	NUNIT_ASSERT( ipport_aview_t().is_null() );
	
	// set a local view and check it is not null
	NUNIT_ASSERT( !ipport_aview_t().lview("127.0.0.1:4000").is_null() );
	
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the comparison operator
 */
nunit_res_t	ipport_aview_testclass_t::xmlrpc_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ipport_aview_t	ipport_aview_toserial;
	ipport_aview_t	ipport_aview_unserial;
	// log to debug
	KLOG_DBG("enter");

	// set the ipport_aview_toserial with a null public view
	ipport_aview_toserial.lview("127.0.0.1:4000");

// build the xmlrpc
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("examples.getStateName");
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG;
	xmlrpc_build		<< ipport_aview_toserial;
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;
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
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEG;
		xmlrpc_parse		>> ipport_aview_unserial;
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_END;
		xmlrpc_parse >> xmlrpc_parse_t::CALL_END;
	} catch(xml_except_t &e){
		KLOG_ERR("xml_except_t=" << e.what());
		NUNIT_ASSERT(0);
	}
	// check the parameters have the expected values
	NUNIT_ASSERT( method_name == "examples.getStateName" );	
	NUNIT_ASSERT( ipport_aview_toserial == ipport_aview_unserial );	
	
	
	// return no error
	return NUNIT_RES_OK;
}
NEOIP_NAMESPACE_END

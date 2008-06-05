/*! \file
    \brief Definition of the unit test for the \ref xml_parse_t

*/

/* system include */
/* local include */
#include "neoip_xml_parse_nunit.hpp"
#include "neoip_xml_parse_doc.hpp"
#include "neoip_xml_parse.hpp"
#include "neoip_file_path.hpp"
#include "neoip_datum.hpp"
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
nunit_res_t	xml_parse_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// set the document to parse
	xml_parse_doc_t	xml_parse_doc;
	xml_parse_doc.set_document(datum_t("<?xml version=\"1.0\"?>\n<myownroot version=\"3.0\">myrootvalue<myfirstchild>mychildvalue</myfirstchild></myownroot>\n"));
	// check that the parsing suceed
	NUNIT_ASSERT( !xml_parse_doc.is_null() );

	// init the xml_parse_t on this document
	xml_parse_t	xml_parse(&xml_parse_doc);
	
	// parse the document
	try {
		xml_parse.goto_root();
		NUNIT_ASSERT(xml_parse.node_name() == "myownroot");
		NUNIT_ASSERT(xml_parse.node_attr("version") == "3.0");
		NUNIT_ASSERT(xml_parse.node_content() == "myrootvalue" );
		xml_parse.goto_children().goto_nextsib();
		NUNIT_ASSERT(xml_parse.node_name() == "myfirstchild" );
		NUNIT_ASSERT(xml_parse.node_content() == "mychildvalue" );
	} catch(xml_except_t &e){
		NUNIT_ASSERT(0);
	}

	// log to debug
	KLOG_DBG("myfirstchild content=" << xml_parse.path_content_opt("/myownroot/myfirstchild"));
	// check the xml_parse_t::has_path with an absolute path
	NUNIT_ASSERT( xml_parse.has_path("/myownroot/myfirstchild") );
	// check the xml_parse_t::has_path with an relative path
	NUNIT_ASSERT( xml_parse.goto_root().goto_children().has_path("myfirstchild") );

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END


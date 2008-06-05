/*! \file
    \brief Definition of the unit test for the \ref mlink_t

*/

/* system include */
/* local include */
#include "neoip_mlink_nunit.hpp"
#include "neoip_mlink_file.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_err.hpp"
#include "neoip_xml_parse_doc.hpp"
#include "neoip_xml_parse.hpp"
#include "neoip_xml_build.hpp"
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
nunit_res_t	mlink_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_path_t	file_path;
	datum_t		xml_datum;
	file_path	= "/home/jerome/downloaded/ubuntu-6_06_1-desktop-i386_iso.metalink";
//	file_path	= "/tmp/nlite1.0fi.exe.metalink";
	// log to debug
	KLOG_WARN("enter");	

	// read the whole metalink file
	file_err_t	file_err;
	file_err	= file_sio_t::readall(file_path, xml_datum);
	NUNIT_ASSERT( file_err.succeed() );

	// setup the xml_parse_t
	xml_parse_doc_t	xml_parse_doc;
	xml_parse_doc.set_document(xml_datum);
	NUNIT_ASSERT( !xml_parse_doc.is_null() );	

	// init the xml_parse_t on this document
	xml_parse_t	xml_parse(&xml_parse_doc);
	
	// build the mlink_t from the xml_parse_t
	mlink_file_t	mlink_file;
	// parse the document into a mlink_file_t
	try {
		xml_parse	>> mlink_file;
	} catch(xml_except_t &e){
		NUNIT_ASSERT(0);
	}
	
	// log to debug
	KLOG_DBG("mlink_file=" << mlink_file);
	
	xml_build_t	xml_build;
	xml_build	<< mlink_file;
	KLOG_WARN("mlink_file=" << xml_build.to_stdstring());
	
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END


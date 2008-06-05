/*! \file
    \brief Definition of the unit test for the \ref xml_build_t

*/

/* system include */
/* local include */
#include "neoip_xml_build_nunit.hpp"
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
nunit_res_t	xml_build_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	xml_build_t	xml_build;	
	// log to debug
	KLOG_DBG("enter");	

	// build a xml document
	xml_build.element_beg("myownroot").element_att("version", "3.0");
	xml_build	.element_val("myrootvalue");
	xml_build	.element_beg("myfirstchild");
	xml_build		.element_val("mychildval<br>ue");
	xml_build	.element_end();
	xml_build.element_end();

	// log to debug	
	KLOG_ERR("xml_build=" << xml_build);
	
	// check the result datum_t matches the theorical result
	NUNIT_ASSERT( xml_build.to_stdstring() == "<?xml version=\"1.0\"?>\n<myownroot version=\"3.0\">myrootvalue<myfirstchild>mychildvalue</myfirstchild></myownroot>\n");

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END


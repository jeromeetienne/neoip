/*! \file
    \brief Definition of the unit test for the \ref strvar_db_t
*/

/* system include */
/* local include */
#include "neoip_strvar_db_nunit.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_strvar_helper.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief function to test a strvar_db_t
 */
nunit_res_t	strvar_db_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	strvar_db_t	strvar_db;
	// log to debug
	KLOG_DBG("enter");

	strvar_db.append("testkey1", "testval1");
	NUNIT_ASSERT( strvar_db[0] == strvar_item_t("testkey1", "testval1") );

	strvar_db.append("testkey2", "testval2");
	NUNIT_ASSERT( strvar_db[1] == strvar_item_t("testkey2", "testval2") );

	strvar_db.append("testkey2", "testval2-dups");
	NUNIT_ASSERT( strvar_db[2] == strvar_item_t("testkey2", "testval2-dups") );

	strvar_db.update("testkey2", "testval2-new");
	NUNIT_ASSERT( strvar_db[1] == strvar_item_t("testkey2", "testval2-new") );

	// test the key_ignorecase()
	NUNIT_ASSERT( strvar_db.contain_key("TestKEY1") == false );	
	strvar_db.key_ignorecase(true);
	NUNIT_ASSERT( strvar_db.contain_key("TestKEY1") == true );	
	NUNIT_ASSERT( strvar_db.get_first_value("TestKEY1") == "testval1" );	

#if 0
	strvar_db	= strvar_helper_t::from_file("/home/jerome/workspace/yavipin/src/conf_router_itor/neoip_log.conf");
	KLOG_ERR("strvar_db=" << strvar_db);
#endif
	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END


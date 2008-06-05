/*! \file
    \brief unit test for the ndiag_cacheport_t
*/

/* system include */
/* local include */
#include "neoip_ndiag_cacheport_nunit.hpp"
#include "neoip_ndiag_cacheport.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the init stuff
 */
nunit_res_t	ndiag_cacheport_testclass_t::init(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// log to debug
	KLOG_DBG("enter");
	// create the object from the default ctor
	ndiag_cacheport_t	ndiag_cacheport;
	// the object from the default ctor MUST be null
	NUNIT_ASSERT( ndiag_cacheport.is_null() );
	// the object from the default ctor MUST be empty
	NUNIT_ASSERT( ndiag_cacheport.empty() );
	// the object from the default ctor MUST have a size of 0
	NUNIT_ASSERT( ndiag_cacheport.size() == 0 );
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the contain() stuff
 */
nunit_res_t	ndiag_cacheport_testclass_t::contain(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// log to debug
	KLOG_DBG("enter");
	// create the object from the default ctor
	ndiag_cacheport_t	ndiag_cacheport;
	// test the ndiag_cacheport_id_t IS NOT contained
	NUNIT_ASSERT( ndiag_cacheport.contain("hello", upnp_sockfam_t::TCP) == false );
	// add an item
	ndiag_cacheport.update("hello", upnp_sockfam_t::TCP, 1234);
	// test the ndiag_cacheport_id_t IS contained
	NUNIT_ASSERT( ndiag_cacheport.contain("hello", upnp_sockfam_t::TCP) );
	// test the ndiag_cacheport_item_t return the proper port
	NUNIT_ASSERT( ndiag_cacheport.item("hello", upnp_sockfam_t::TCP).port() == 1234 );
	// return no error
	return NUNIT_RES_OK;
}


/** \brief test the contain() stuff
 */
nunit_res_t	ndiag_cacheport_testclass_t::update_remove(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// log to debug
	KLOG_DBG("enter");
	// create the object from the default ctor
	ndiag_cacheport_t	ndiag_cacheport;
	// add an item
	ndiag_cacheport.update("hello", upnp_sockfam_t::TCP, 1234);
	// add an item
	ndiag_cacheport.update("world", upnp_sockfam_t::TCP, 5678);
	// test the ndiag_cacheport_id_t are contained
	NUNIT_ASSERT( ndiag_cacheport.contain("hello", upnp_sockfam_t::TCP) );
	NUNIT_ASSERT( ndiag_cacheport.item("hello", upnp_sockfam_t::TCP).port() == 1234 );
	NUNIT_ASSERT( ndiag_cacheport.contain("world", upnp_sockfam_t::TCP) );
	NUNIT_ASSERT( ndiag_cacheport.item("world", upnp_sockfam_t::TCP).port() == 5678 );
	// test the size() == 2
	NUNIT_ASSERT( ndiag_cacheport.size() == 2 );
	// remove an item
	ndiag_cacheport.remove("hello", upnp_sockfam_t::TCP);
	// test the size() == 1
	NUNIT_ASSERT( ndiag_cacheport.size() == 1 );
	// test that "hello" is no more contained
	NUNIT_ASSERT( ndiag_cacheport.contain("hello", upnp_sockfam_t::TCP) == false );
	// test that "world" is still contained
	NUNIT_ASSERT( ndiag_cacheport.contain("world", upnp_sockfam_t::TCP) );
	NUNIT_ASSERT( ndiag_cacheport.item("world", upnp_sockfam_t::TCP).port() == 5678 );
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

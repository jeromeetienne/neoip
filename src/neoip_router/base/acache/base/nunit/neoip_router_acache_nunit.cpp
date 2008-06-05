/*! \file
    \brief Definition of the unit test for the router_acache_t
    
*/

/* system include */
/* local include */
#include "neoip_router_acache_nunit.hpp"
#include "neoip_router_acache.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_acache_testclass_t::router_acache_testclass_t()	throw()
{
	// zero some field
	router_acache	= NULL;
}

/** \brief Destructor
 */
router_acache_testclass_t::~router_acache_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	router_acache_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// init router_acache_t
	router_acache	= nipmem_new router_acache_t();
	acache_bytearray= bytearray_t();
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	router_acache_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// stop the expire_timeout if needed
	if( expire_timeout.is_running() )	expire_timeout.stop();
	// delete router_acache_t if needed
	nipmem_zdelete	router_acache;
	// reset the acache_bytearray
	acache_bytearray= bytearray_t();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function for item presence
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test is an item do expire
 */
nunit_res_t	router_acache_testclass_t::item_presence(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	const router_acache_item_t *	item;
	// log to debug
	KLOG_DBG("enter");
	
	// test if the item is NOT present
	item	= router_acache->find_by_remote_iaddr("127.0.0.2");
	NUNIT_ASSERT( !item );
	item	= router_acache->find_by_remote_dnsname(router_name_t("myhost.mydomain"));
	NUNIT_ASSERT( !item );
	// check the size
	NUNIT_ASSERT( router_acache->size() == 0 );

	// add an item in the router_acache
	router_acache->update(router_name_t("myhost.mydomain"), "127.0.0.1", "127.0.0.2"
						, delay_t::from_msec(100));

	// check the size
	NUNIT_ASSERT( router_acache->size() == 1 );
	NUNIT_ASSERT( (*router_acache)[0].local_iaddr() == "127.0.0.1"); 

	// test if the item is present
	item	= router_acache->find_by_remote_iaddr("127.0.0.2");
	NUNIT_ASSERT( item && item->remote_dnsname() == router_name_t("myhost.mydomain") );
	item	= router_acache->find_by_remote_dnsname(router_name_t("myhost.mydomain"));
	NUNIT_ASSERT( item && item->remote_iaddr() == "127.0.0.2" );

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to wait for bootstrap
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test is an item do expire
 */
nunit_res_t	router_acache_testclass_t::item_expiration(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// ensure it is still present from the previous test function
	NUNIT_ASSERT( router_acache->find_by_remote_iaddr("127.0.0.2") );
	// log to debug
	KLOG_ERR("router_accahe=" << *router_acache);
	// serialize the current router_acache_t to test the expiration IN the bytearrayt_t
	acache_bytearray << *router_acache;

	// Start the timeout
	expire_timeout.start(delay_t::from_msec(200), this, NULL);
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback called when the timeout_t expire
 */
bool 	router_acache_testclass_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// stop the timeout
	expire_timeout.stop();
	
	// if the item is still present, notify a nunit error
	if( router_acache->find_by_remote_iaddr("127.0.0.2") )
		return nunit_ftor(NUNIT_RES_ERROR);

	// unserialize the acache_serial to check if the item DID expire in it
	router_acache_t	unserialized_acache;
	try {
		acache_bytearray >> unserialized_acache;
		if( unserialized_acache.find_by_remote_iaddr("127.0.0.2") )
			return nunit_ftor(NUNIT_RES_ERROR);
	}catch(serial_except_t &e){
		return nunit_ftor(NUNIT_RES_ERROR);
	}	
		
	// report the result
	return nunit_ftor(NUNIT_RES_OK);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function for item presence
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test is an item do expire
 */
nunit_res_t	router_acache_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	router_acache_t	acache_toserial;
	router_acache_t	acache_unserial;
	serial_t	serial;
	const router_acache_item_t *	item;
	// set ntudp_addr_toserial
	acache_toserial.update(router_name_t("dnsname1.mydomain"), "127.0.0.1", "127.0.0.2", delay_t::from_sec(10));
	acache_toserial.update(router_name_t("dnsname2.mydomain"), "127.0.0.3", "127.0.0.4", delay_t::from_sec(10));
	// do the serial/unserial
	serial << acache_toserial;
	serial >> acache_unserial;
	// note: router_acache_t has no comparison operator
	
	// check the size
	NUNIT_ASSERT( acache_toserial.size() == acache_unserial.size() );

	// test if the item is present
	item	= acache_unserial.find_by_remote_iaddr("127.0.0.2");
	NUNIT_ASSERT( item && item->remote_dnsname() == router_name_t("dnsname1.mydomain") );
	item	= acache_unserial.find_by_remote_dnsname(router_name_t("dnsname2.mydomain"));
	NUNIT_ASSERT( item && item->remote_iaddr() == "127.0.0.4" );
	
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END


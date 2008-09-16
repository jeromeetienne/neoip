/*! \file
    \brief unit test for the nslan_rec_src_t unit test

\par Possible Improvements
- this unit test is way to basic.
  - a lot of thing may go wrong and this test will still pass
  - TODO find a good test and code it

*/

/* system include */
/* local include */
#include "neoip_nslan_rec_src_nunit.hpp"
#include "neoip_nslan_rec_src.hpp"
#include "neoip_nslan.hpp"
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
nslan_rec_src_testclass_t::nslan_rec_src_testclass_t()	throw()
{
	// zero some field
	nslan_listener	= NULL;
	nslan_peer	= NULL;
	nslan_publish	= NULL;
	nslan_rec_src	= NULL;
}

/** \brief Destructor
 */
nslan_rec_src_testclass_t::~nslan_rec_src_testclass_t()	throw()
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
nunit_err_t	nslan_rec_src_testclass_t::neoip_nunit_testclass_init()	throw()
{
	inet_err_t inet_err;
	// log to debug
	KLOG_DBG("enter");
	
	// sanity check
	DBG_ASSERT( !nslan_listener );
	
	// create a nslan_listener_t
	nslan_listener	= nipmem_new nslan_listener_t();
	inet_err	= nslan_listener->start("255.255.255.255:4000");
	if( inet_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, inet_err.to_string());
	
	// create a nslan_peer_t
	nslan_realmid_t	realmid	= "nunit_realmid";
	nslan_peer	= nipmem_new nslan_peer_t(nslan_listener, "nunit_realm");

	// create a nslan_rec_t
	nslan_keyid_t	keyid		= "nunid_keyid";
	datum_t		payload("bonjour", 7);
	nslan_rec_t	nslan_rec(keyid, payload, delay_t::from_sec(20));

	// publish the record
	nslan_publish	= nipmem_new nslan_publish_t(nslan_peer, nslan_rec);

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	nslan_rec_src_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// delete the nslan_rec_src
	if( nslan_rec_src ){
		nipmem_delete	nslan_rec_src;
		nslan_rec_src	= NULL;
	}
	// delete the nslan_publish
	if( nslan_publish ){
		nipmem_delete	nslan_publish;
		nslan_publish	= NULL;
	}
	// delete the nslan_peer
	if( nslan_peer ){
		nipmem_delete	nslan_peer;
		nslan_peer	= NULL;
	}
	// delete the nslan_listener
	if( nslan_listener ){
		nipmem_delete	nslan_listener;
		nslan_listener	= NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			nslan_rec_src_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref nslan_rec_src_t when to notify an ipport_addr_t
 */
bool nslan_rec_src_testclass_t::neoip_nslan_rec_src_cb(void *cb_userptr, nslan_rec_src_t &cb_nslan_rec_src
			, const nslan_rec_t &nslan_rec, const ipport_addr_t &src_addr)	throw()
{
	// log to debug
	KLOG_ERR("enter notified record=" << nslan_rec << " src_addr=" << src_addr);

	// if the notified_record is not the expected one, ignore it and go on the test
	if( nslan_rec != nslan_publish->get_record() )	return true;

	// delete the nslan_rec_src and mark it unused
	nipmem_delete	nslan_rec_src;
	nslan_rec_src	= NULL;
	// notify the caller of the end of this testfunction
	nunit_ftor(NUNIT_RES_OK);
	// return 'dontkeep'
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	nslan_rec_src_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// sanity check
	DBG_ASSERT( !nslan_rec_src );
	DBG_ASSERT( nslan_peer );
	// Start the nslan_rec_src
	inet_err_t	inet_err;
	nslan_rec_src	= nipmem_new nslan_rec_src_t();
	inet_err	= nslan_rec_src->start(nslan_peer, nslan_publish->get_record().get_keyid()
								, this, NULL);
	NUNIT_ASSERT( inet_err.succeed() );
	// request one address
	nslan_rec_src->get_more();
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}



NEOIP_NAMESPACE_END

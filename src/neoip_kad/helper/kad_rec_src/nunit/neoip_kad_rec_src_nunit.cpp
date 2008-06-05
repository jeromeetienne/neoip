/*! \file
    \brief unit test for the diffie-hellman unit test

\par Possible Improvements
- this unit test is way to basic.
  - a lot of thing may go wrong and this test will still pass
  - TODO find a good test and code it

*/

/* system include */
/* local include */
#include "neoip_kad_rec_src_nunit.hpp"
#include "neoip_kad_rec_src.hpp"
#include "neoip_kad_listener.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_kad_store.hpp"
#include "neoip_udp_vresp.hpp"
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
kad_rec_src_testclass_t::kad_rec_src_testclass_t()	throw()
{
	// zero some field
	udp_vresp	= NULL;
	kad_listener	= NULL;
	kad_peer	= NULL;
	kad_store	= NULL;
	kad_rec_src	= NULL;
}

/** \brief Destructor
 */
kad_rec_src_testclass_t::~kad_rec_src_testclass_t()	throw()
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
nunit_err_t	kad_rec_src_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ipport_aview_t	listen_aview	= ipport_aview_t().lview("0.0.0.0:4000");	
	// log to debug
	KLOG_DBG("enter");
	
	// sanity check
	DBG_ASSERT( !kad_listener );

	// init the udp_vresp_t for the kad_listener
	inet_err_t	inet_err;
	udp_vresp	= nipmem_new udp_vresp_t();
	inet_err	= udp_vresp->start(listen_aview.lview());
	if( inet_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, inet_err.to_string() );

	// init kad_listener
	kad_err_t	kad_err;
	kad_listener	= nipmem_new kad_listener_t();
	kad_err 	= kad_listener->start("255.255.255.255:7777", udp_vresp, listen_aview);
	if( kad_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, kad_err.to_string() );


	// add a peer in the kad_listener
	kad_realmid_t	realmid	= "kad_rec_src_nunit_realm";	// the realm_id for this unittest
	kad_peerid_t	peerid	= kad_peerid_t::build_random();
	kad_peer	= nipmem_new kad_peer_t();
	kad_err		= kad_peer->start(kad_listener, realmid, peerid);
	if( kad_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, kad_err.to_string() );

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	kad_rec_src_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	nipmem_zdelete kad_rec_src;
	nipmem_zdelete kad_store;
	nipmem_zdelete kad_peer;
	nipmem_zdelete kad_listener;
	nipmem_zdelete udp_vresp;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit test for the store_record
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	kad_rec_src_testclass_t::store_record(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// sanity check
	DBG_ASSERT( !kad_rec_src );
	DBG_ASSERT( kad_listener );

	// build a record to publish
	keyid		= std::string("record of kad_rec_src_nunit");
	kad_recid_t	recid	= kad_recid_t::build_random();	
	datum_t		payload	= datum_t("data for kad_rec_src_nunit");
	kad_rec_t	kad_rec = kad_rec_t(recid, keyid, delay_t::from_sec(60), payload);
	// publish a record in this peer
	kad_store	= nipmem_new kad_store_t();
	kad_err		= kad_store->start(kad_peer, kad_rec, delay_t::from_sec(30), this, NULL);
	if( kad_err.failed() )	return NUNIT_RES_ERROR;
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when kad_store_t has an event to notify
 */
bool	kad_rec_src_testclass_t::neoip_kad_store_cb(void *cb_userptr, kad_store_t &cb_kad_store
						, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << kad_event);
	// sanity check - the kad_event MUST be store_ok()
	DBG_ASSERT( kad_event.is_store_ok() );
	
	// determine if the test is positif of negative	
	bool	succeed	= kad_event.is_completed();

	// delete the kad_store_t and mark it unused
	nipmem_zdelete	kad_store;

	// notify the caller of the end of this testfunction
	if( succeed )	nunit_ftor(NUNIT_RES_OK);
	else		nunit_ftor(NUNIT_RES_ERROR);

	// return 'dontkeep'
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit test for the query
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	kad_rec_src_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// sanity check
	DBG_ASSERT( !kad_rec_src );
	DBG_ASSERT( kad_listener );
	
	// Start the kad_rec_src
	kad_err_t	kad_err;
	kad_rec_src	= nipmem_new kad_rec_src_t();
	kad_err		= kad_rec_src->start(kad_peer, keyid, this, NULL);
	NUNIT_ASSERT( kad_err.succeed() );
	// request one address
	kad_rec_src->get_more();
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback notified by \ref kad_rec_src_t when to notify an ipport_addr_t
 */
bool kad_rec_src_testclass_t::neoip_kad_rec_src_cb(void *cb_userptr, kad_rec_src_t &cb_kad_rec_src
						, const kad_rec_t &kad_rec)	throw()
{
	// log to debug
	KLOG_DBG("enter notified record=" << kad_rec);

	kad_rec_t	tmp	= kad_rec;

	// delete the kad_rec_src and mark it unused
	nipmem_zdelete	kad_rec_src;

	// notify the caller of the end of this testfunction
	if( tmp.is_null() )	nunit_ftor(NUNIT_RES_ERROR);
	else			nunit_ftor(NUNIT_RES_OK);
	// return 'dontkeep'
	return false;
}


NEOIP_NAMESPACE_END

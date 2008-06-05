/*! \file
    \brief Definition of the unit test for the \ref kad_listener_t


\par List of tests
- ping_reach_peer: a kad_ping_rpc_t which MUST return completed
- ping_unreach_peer: a kad_ping_rpc_t which MUST return fatal
- findnode: a kad_findnode_rpc_t which MUST return a kad_addr_arr_t
- findsomeval_fornode: a kad_findnode_rpc_t which MUST return a kad_addr_arr_t
- store_record: kad_store_rpc_t which MUST return completed
- findsomeval_forval: kad_findsomeval_t MUST return a kad_reclist_t containing the published record
- findallval_found: kad_findallval_rpc_t which MUST return a kad_reclist_t containing the published record
- delete_record: a kad_delete_rpc_t which MUST return completed
- findallval_notfound: a kad_findallval_rpc_t which MUST return an empty kad_reclist_t

*/

/* system include */
/* local include */
#include "neoip_kad_client_rpc_nunit.hpp"
#include "neoip_kad_listener.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_kad_ping_rpc.hpp"
#include "neoip_kad_findnode_rpc.hpp"
#include "neoip_kad_findsomeval_rpc.hpp"
#include "neoip_kad_store_rpc.hpp"
#include "neoip_kad_findallval_rpc.hpp"
#include "neoip_kad_delete_rpc.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_client_rpc_testclass_t::kad_client_rpc_testclass_t()	throw()
{
	// zero some field
	kad_listener		= NULL;
	kad_peer		= NULL;
	udp_vresp		= NULL;
	
	ping_doreach_peer	= NULL;
	ping_unreach_peer	= NULL;
	findnode		= NULL;
	findsomeval_fornode	= NULL;
	store			= NULL;
	findsomeval_forval	= NULL;
	findallval_do_found	= NULL;
	delete_record		= NULL;
	findallval_notfound	= NULL;
}

/** \brief Destructor
 */
kad_client_rpc_testclass_t::~kad_client_rpc_testclass_t()	throw()
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
nunit_err_t	kad_client_rpc_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ipport_aview_t	listen_aview	= ipport_aview_t().lview("0.0.0.0:4000").pview("127.0.0.1:4000");
	kad_err_t	kad_err;

	// init the udp_vresp_t for the kad_listener
	inet_err_t	inet_err;
	udp_vresp	= nipmem_new udp_vresp_t();
	inet_err	= udp_vresp->start(listen_aview.lview());
	if( inet_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, inet_err.to_string());

	// init kad_listener
	kad_listener		= nipmem_new kad_listener_t();
	kad_err = kad_listener->start("255.255.255.255:7777", udp_vresp, listen_aview);
	if( kad_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, kad_err.to_string());


	// set some variables for kad_peer_t
	kad_realmid_t	realmid	= "kad_listener_nunit_realm";	// the realm_id for this unittest
	kad_peerid_t	peerid	= kad_peerid_t::build_random();
	// create the kad_peer_t
	kad_peer	= nipmem_new kad_peer_t();
	kad_err		= kad_peer->start(kad_listener, realmid, peerid);
	DBG_ASSERT( kad_err.succeed() );

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	kad_client_rpc_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	nipmem_zdelete	ping_doreach_peer;
	nipmem_zdelete	ping_unreach_peer;
	nipmem_zdelete	findnode;
	nipmem_zdelete	findsomeval_fornode;
	nipmem_zdelete	store;
	nipmem_zdelete	findsomeval_forval;
	nipmem_zdelete	findallval_do_found;
	nipmem_zdelete	delete_record;
	nipmem_zdelete	findallval_notfound;

	nipmem_zdelete	kad_peer;
	nipmem_zdelete	kad_listener;
	nipmem_zdelete	udp_vresp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   callback dispatchers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_ping_rpc_t has an event to notify
 * 
 * - simply route the kad_event_t to the proper subcallback
 */
bool	kad_client_rpc_testclass_t::neoip_kad_ping_rpc_cb(void *cb_userptr
						, kad_ping_rpc_t &cb_kad_ping_rpc
						, const kad_event_t &kad_event) throw()
{
	if( ping_doreach_peer == &cb_kad_ping_rpc )	return ping_doreach_peer_cb(cb_userptr, cb_kad_ping_rpc, kad_event);
	if( ping_unreach_peer == &cb_kad_ping_rpc )	return ping_unreach_peer_cb(cb_userptr, cb_kad_ping_rpc, kad_event);
	// NOTE: this point MUST NOT be reached
	DBG_ASSERT( 0 );
	return false;
}

/** \brief callback notified when a kad_findsomeval_rpc_t has an event to notify
 * 
 * - simply route the kad_event_t to the proper subcallback
 */
bool	kad_client_rpc_testclass_t::neoip_kad_findsomeval_rpc_cb(void *cb_userptr
						, kad_findsomeval_rpc_t &cb_kad_findsomeval_rpc
						, const kad_event_t &kad_event) throw()
{
	if( findsomeval_fornode == &cb_kad_findsomeval_rpc )	return findsomeval_fornode_cb(cb_userptr, cb_kad_findsomeval_rpc, kad_event);
	if( findsomeval_forval == &cb_kad_findsomeval_rpc )	return findsomeval_forval_cb(cb_userptr, cb_kad_findsomeval_rpc, kad_event);
	// NOTE: this point MUST NOT be reached
	DBG_ASSERT( 0 );
	return false;
}

/** \brief callback notified when a kad_findallval_rpc_t has an event to notify
 * 
 * - simply route the kad_event_t to the proper subcallback
 */
bool	kad_client_rpc_testclass_t::neoip_kad_findallval_rpc_cb(void *cb_userptr
						, kad_findallval_rpc_t &cb_kad_findallval_rpc
						, const kad_event_t &kad_event) throw()
{
	if( findallval_do_found == &cb_kad_findallval_rpc )	return findallval_do_found_cb(cb_userptr, cb_kad_findallval_rpc, kad_event);
	if( findallval_notfound == &cb_kad_findallval_rpc )	return findallval_notfound_cb(cb_userptr, cb_kad_findallval_rpc, kad_event);
	// NOTE: this point MUST NOT be reached
	DBG_ASSERT( 0 );
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function test_ping_doreach_peer
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test a kad_ping_rpc_t toward a reachable kad_peer_t
 */
nunit_res_t	kad_client_rpc_testclass_t::test_ping_doreach_peer(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// compute the dest_addr for kad_peer_t from a localhost point of view
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_ERR("enter");

	// sanity check - the ping_doreach_peer MUST be NULL
	DBG_ASSERT( ping_doreach_peer == NULL );
	
	// create and start the ping_doreach_peer rpc toward the launched kad_peer_t
	ping_doreach_peer	= nipmem_new kad_ping_rpc_t();
	kad_err			= ping_doreach_peer->start(kad_peer, dest_addr, delay_t::from_sec(10)
										, this, NULL);
	NUNIT_ASSERT( kad_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_ping_rpc_t has an event to notify
 */
bool	kad_client_rpc_testclass_t::ping_doreach_peer_cb(void *cb_userptr
			, kad_ping_rpc_t &cb_kad_ping_rpc, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_DBG("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be ping_rpc_ok
	DBG_ASSERT( kad_event.is_ping_rpc_ok() );
	
	// notify the nunit_res_t depending on the kad_event
	bool	tokeep;
	if( kad_event.is_completed() )	tokeep	= nunit_ftor(NUNIT_RES_OK);
	else				tokeep	= nunit_ftor(NUNIT_RES_ERROR);
	if( !tokeep )	return false;

	// delete the notifier and markit unused
	nipmem_zdelete	ping_doreach_peer;

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function test_ping_unreach_peer
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test a kad_ping_rpc_t toward a reachable kad_peer_t
 */
nunit_res_t	kad_client_rpc_testclass_t::test_ping_unreach_peer(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// compute a random dest_addr known to be unreachable
	kad_addr_t	dest_addr("127.42.42.42:4242", kad_peerid_t::build_random());
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the ping_unreach_peer MUST be NULL
	DBG_ASSERT( ping_unreach_peer == NULL );
	
	// create and start the ping_unreach_peer rpc toward a random destination
	ping_unreach_peer	= nipmem_new kad_ping_rpc_t();
	kad_err			= ping_unreach_peer->start(kad_peer, dest_addr
							, delay_t::from_sec(10), this, NULL);
	NUNIT_ASSERT( kad_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_ping_rpc_t has an event to notify
 */
bool	kad_client_rpc_testclass_t::ping_unreach_peer_cb(void *cb_userptr
			, kad_ping_rpc_t &cb_kad_ping_rpc, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_DBG("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be ping_rpc_ok
	DBG_ASSERT( kad_event.is_ping_rpc_ok() );
	
	// notify the nunit_res_t depending on the kad_event
	bool	tokeep;
	if( kad_event.is_fatal() )	tokeep	= nunit_ftor(NUNIT_RES_OK);
	else				tokeep	= nunit_ftor(NUNIT_RES_ERROR);
	if( !tokeep )	return false;

	// delete the notifier and markit unused
	nipmem_zdelete	ping_unreach_peer;

	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function test_findnode
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test a kad_findnode_rpc_t toward a reachable kad_peer_t
 */
nunit_res_t	kad_client_rpc_testclass_t::test_findnode(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// compute the dest_addr for kad_peer_t from a localhost point of view
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the findnode MUST be NULL
	DBG_ASSERT( findnode == NULL );
	
	// create and start the findnode rpc toward a random destination
	findnode	= nipmem_new kad_findnode_rpc_t();
	kad_err		= findnode->start(kad_peer, kad_targetid_t::build_any(), dest_addr
							, delay_t::from_sec(10), this, NULL);
	NUNIT_ASSERT( kad_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_findnode_rpc_t has an event to notify
 */
bool	kad_client_rpc_testclass_t::neoip_kad_findnode_rpc_cb(void *cb_userptr
					, kad_findnode_rpc_t &cb_kad_findnode_rpc
					, const kad_event_t &kad_event) throw()
{
	nunit_res_t	nunit_res	= NUNIT_RES_ERROR;
	// compute the dest_addr for kad_peer_t from a localhost point of view
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be findnode_rpc_ok
	DBG_ASSERT( kad_event.is_findnode_rpc_ok() );

	// notify the nunit_res_t depending on the kad_event
	if( kad_event.is_addr_arr() ){
		const kad_addr_arr_t &	kad_addr_arr = kad_event.get_addr_arr(NULL);
		if( kad_addr_arr.size() == 1 && kad_addr_arr[0] == dest_addr ){
			nunit_res	= NUNIT_RES_OK;
		}
	}
	// notify the result
	bool	tokeep	= nunit_ftor(nunit_res);
	if( !tokeep )	return false;

	// delete the notifier and markit unused
	nipmem_zdelete	findnode;

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function test_findsomeval_fornode
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test a kad_findsomeval_rpc_t toward a reachable kad_peer_t
 */
nunit_res_t	kad_client_rpc_testclass_t::test_findsomeval_fornode(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// compute the dest_addr for kad_peer_t from a localhost point of view
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the findsomeval_fornode MUST be NULL
	DBG_ASSERT( findsomeval_fornode == NULL );
	
	// create and start the findsomeval rpc toward a random destination
	findsomeval_fornode	= nipmem_new kad_findsomeval_rpc_t();
	kad_err			= findsomeval_fornode->start(kad_peer, kad_targetid_t::build_any(), 42
							, dest_addr, delay_t::from_sec(10), this, NULL);
	NUNIT_ASSERT( kad_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_findsomeval_rpc_t has an event to notify
 */
bool	kad_client_rpc_testclass_t::findsomeval_fornode_cb(void *cb_userptr
					, kad_findsomeval_rpc_t &cb_kad_findsomeval_rpc
					, const kad_event_t &kad_event) throw()
{
	nunit_res_t	nunit_res	= NUNIT_RES_ERROR;
	// compute the dest_addr for kad_peer_t from a localhost point of view	
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be findsomeval_rpc_ok
	DBG_ASSERT( kad_event.is_findsomeval_rpc_ok() );

	// notify the nunit_res_t depending on the kad_event
	if( kad_event.is_addr_arr() ){
		const kad_addr_arr_t &	kad_addr_arr = kad_event.get_addr_arr(&cookie_id);
		if( kad_addr_arr.size() == 1 && kad_addr_arr[0] == dest_addr ){
			nunit_res	= NUNIT_RES_OK;
		}
	}
	// notify the result
	bool	tokeep	= nunit_ftor(nunit_res);
	if( !tokeep )	return false;

	// delete the notifier and markit unused
	nipmem_zdelete	findsomeval_fornode;

	// return dontkeep
	return false;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function test_store
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test a kad_store_rpc_t toward a reachable kad_peer_t
 */
nunit_res_t	kad_client_rpc_testclass_t::test_store(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// compute the dest_addr for kad_peer_t from a localhost point of view	
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the store MUST be NULL
	DBG_ASSERT( store == NULL );

	// build a record to publish
	kad_keyid_t	keyid	= "keyid for client_rpc_nunit record";
	kad_recid_t	recid	= "recid for client_rpc_nunit record";
	datum_t		payload	= datum_t("dummy data payload");
	kad_rec_t	kad_rec = kad_rec_t(recid, keyid, delay_t::from_sec(60), payload);
		
	// create and start the store rpc toward a random destination
	store	= nipmem_new kad_store_rpc_t();
	kad_err	= store->start(kad_peer, kad_rec, cookie_id, dest_addr, delay_t::from_sec(10), this,NULL);
	NUNIT_ASSERT( kad_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_store_rpc_t has an event to notify
 */
bool	kad_client_rpc_testclass_t::neoip_kad_store_rpc_cb(void *cb_userptr
					, kad_store_rpc_t &cb_kad_store_rpc
					, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be store_rpc_ok
	DBG_ASSERT( kad_event.is_store_rpc_ok() );

	// notify the nunit_res_t depending on the kad_event
	bool	tokeep;
	if( kad_event.is_completed() )	tokeep	= nunit_ftor(NUNIT_RES_OK);
	else				tokeep	= nunit_ftor(NUNIT_RES_ERROR);
	if( !tokeep )	return false;

	// delete the notifier and markit unused
	nipmem_zdelete	store;

	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function test_findsomeval_forval
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test a kad_findsomeval_rpc_t toward a reachable kad_peer_t
 */
nunit_res_t	kad_client_rpc_testclass_t::test_findsomeval_forval(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// compute the dest_addr for kad_peer_t from a localhost point of view	
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the findsomeval_forval MUST be NULL
	DBG_ASSERT( findsomeval_forval == NULL );
	
	// create and start the findsomeval rpc toward a random destination
	findsomeval_forval	= nipmem_new kad_findsomeval_rpc_t();
	kad_err			= findsomeval_forval->start(kad_peer
							, "keyid for client_rpc_nunit record", 42
							, dest_addr, delay_t::from_sec(10), this, NULL);
	NUNIT_ASSERT( kad_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_findsomeval_rpc_t has an event to notify
 */
bool	kad_client_rpc_testclass_t::findsomeval_forval_cb(void *cb_userptr
					, kad_findsomeval_rpc_t &cb_kad_findsomeval_rpc
					, const kad_event_t &kad_event) throw()
{
	nunit_res_t	nunit_res	= NUNIT_RES_ERROR;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be findsomeval_rpc_ok
	DBG_ASSERT( kad_event.is_findsomeval_rpc_ok() );

	// notify the nunit_res_t depending on the kad_event
	if( kad_event.is_recdups() ){
		const kad_recdups_t &	kad_recdups = kad_event.get_recdups(NULL);
		if( kad_recdups.size() == 1 && kad_recdups[0].get_recid() == "recid for client_rpc_nunit record" ){
			nunit_res	= NUNIT_RES_OK;
		}
	}
	// notify the result
	bool	tokeep	= nunit_ftor(nunit_res);
	if( !tokeep )	return false;

	// delete the notifier and markit unused
	nipmem_zdelete	findsomeval_forval;

	// return dontkeep
	return false;
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function test_findallval_do_found
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test a kad_findallval_rpc_t toward a reachable kad_peer_t
 */
nunit_res_t	kad_client_rpc_testclass_t::test_findallval_do_found(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// compute the dest_addr for kad_peer_t from a localhost point of view	
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the findallval_do_found MUST be NULL
	DBG_ASSERT( findallval_do_found == NULL );
	
	// create and start the findallval rpc toward a random destination
	findallval_do_found	= nipmem_new kad_findallval_rpc_t();
	kad_err			= findallval_do_found->start(kad_peer, kad_recid_t()
							, "keyid for client_rpc_nunit record", true, 42
							, dest_addr, delay_t::from_sec(10), this, NULL);
	NUNIT_ASSERT( kad_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_findallval_rpc_t has an event to notify
 */
bool	kad_client_rpc_testclass_t::findallval_do_found_cb(void *cb_userptr
					, kad_findallval_rpc_t &cb_kad_findallval_rpc
					, const kad_event_t &kad_event) throw()
{
	nunit_res_t	nunit_res	= NUNIT_RES_ERROR;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be findallval_rpc_ok
	DBG_ASSERT( kad_event.is_findallval_rpc_ok() );

	// notify the nunit_res_t depending on the kad_event
	if( kad_event.is_recdups() ){
		const kad_recdups_t &	kad_recdups = kad_event.get_recdups(NULL);
		if( kad_recdups.size() == 1 && kad_recdups[0].get_recid() == "recid for client_rpc_nunit record" ){
			nunit_res	= NUNIT_RES_OK;
		}
	}
	// notify the result
	bool	tokeep	= nunit_ftor(nunit_res);
	if( !tokeep )	return false;

	// delete the notifier and markit unused
	nipmem_zdelete	findallval_do_found;

	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function test_delete_record
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test a kad_delete_rpc_t toward a reachable kad_peer_t
 */
nunit_res_t	kad_client_rpc_testclass_t::test_delete_record(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// compute the dest_addr for kad_peer_t from a localhost point of view
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the delete_record MUST be NULL
	DBG_ASSERT( delete_record == NULL );
	
	// create and start the delete_record rpc toward the launched kad_peer_t
	delete_record	= nipmem_new kad_delete_rpc_t();
	kad_err		= delete_record->start(kad_peer, "recid for client_rpc_nunit record"
					, cookie_id, dest_addr, delay_t::from_sec(10), this, NULL);
	NUNIT_ASSERT( kad_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_delete_rpc_t has an event to notify
 */
bool	kad_client_rpc_testclass_t::neoip_kad_delete_rpc_cb(void *cb_userptr
						, kad_delete_rpc_t &cb_kad_delete_rpc
						, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_DBG("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be delete_rpc_ok
	DBG_ASSERT( kad_event.is_delete_rpc_ok() );
	
	// notify the nunit_res_t depending on the kad_event
	bool	tokeep;
	if( kad_event.is_completed() )	tokeep	= nunit_ftor(NUNIT_RES_OK);
	else				tokeep	= nunit_ftor(NUNIT_RES_ERROR);
	if( !tokeep )	return false;

	// delete the notifier and markit unused
	nipmem_zdelete	delete_record;

	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function test_findallval_notfound
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test a kad_findallval_rpc_t toward a reachable kad_peer_t
 */
nunit_res_t	kad_client_rpc_testclass_t::test_findallval_notfound(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// compute the dest_addr for kad_peer_t from a localhost point of view
	kad_addr_t	dest_addr("127.0.0.1:4000", kad_peer->local_peerid());
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the findallval_notfound MUST be NULL
	DBG_ASSERT( findallval_notfound == NULL );
	
	// create and start the findallval rpc toward a random destination
	findallval_notfound	= nipmem_new kad_findallval_rpc_t();
	kad_err			= findallval_notfound->start(kad_peer, kad_recid_t()
							, kad_keyid_t::build_any(), true, 42
							, dest_addr, delay_t::from_sec(10), this, NULL);
	NUNIT_ASSERT( kad_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_findallval_rpc_t has an event to notify
 */
bool	kad_client_rpc_testclass_t::findallval_notfound_cb(void *cb_userptr
					, kad_findallval_rpc_t &cb_kad_findallval_rpc
					, const kad_event_t &kad_event) throw()
{
	nunit_res_t	nunit_res	= NUNIT_RES_ERROR;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be findallval_rpc_ok
	DBG_ASSERT( kad_event.is_findallval_rpc_ok() );

	// notify the nunit_res_t depending on the kad_event
	if( kad_event.is_recdups() ){
		const kad_recdups_t &	kad_recdups = kad_event.get_recdups(NULL);
		if( kad_recdups.size() == 0 ){
			nunit_res	= NUNIT_RES_OK;
		}
	}
	// notify the result
	bool	tokeep	= nunit_ftor(nunit_res);
	if( !tokeep )	return false;

	// delete the notifier and markit unused
	nipmem_zdelete	findallval_notfound;

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END


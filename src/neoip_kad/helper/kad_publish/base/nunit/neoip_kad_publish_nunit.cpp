/*! \file
    \brief Definition of the unit test for the \ref kad_listener_t


\par List of tests
- publish_start: do a kad_publish_t until it report completed
- query_dofound: do a kad_query on the just published record which MUST be found
- publish_stop: delete the kad_publish_t
- query_nofound: do a kad_query on the just published recod which MUST NOT be found

*/

/* system include */
/* local include */
#include "neoip_kad_publish_nunit.hpp"
#include "neoip_kad_listener.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_kad_publish.hpp"
#include "neoip_kad_query.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_publish_testclass_t::kad_publish_testclass_t()	throw()
{
	// zero some field
	kad_listener	= NULL;
	kad_peer	= NULL;
	udp_vresp	= NULL;
	
	kad_publish	= NULL;
	query_dofound	= NULL;
	query_nofound	= NULL;
}

/** \brief Destructor
 */
kad_publish_testclass_t::~kad_publish_testclass_t()	throw()
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
nunit_err_t	kad_publish_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ipport_aview_t	listen_aview	= ipport_aview_t().lview("0.0.0.0:4000").pview("127.0.0.1:4000");
	kad_err_t	kad_err;

	// init the udp_vresp_t for the kad_listener
	inet_err_t	inet_err;
	udp_vresp	= nipmem_new udp_vresp_t();
	inet_err	= udp_vresp->start(listen_aview.lview());
	if( inet_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, inet_err.to_string());

	// init kad_listener
	kad_listener	= nipmem_new kad_listener_t();
	kad_err 	= kad_listener->start("255.255.255.255:7777", udp_vresp, listen_aview);
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
void	kad_publish_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	nipmem_zdelete	kad_publish;
	nipmem_zdelete	query_dofound;
	nipmem_zdelete	query_nofound;

	nipmem_zdelete	kad_peer;
	nipmem_zdelete	kad_listener;
	nipmem_zdelete	udp_vresp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   callback dispatchers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_query_t has an event to notify
 * 
 * - simply route the kad_event_t to the proper subcallback
 */
bool	kad_publish_testclass_t::neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
							, const kad_event_t &kad_event) throw()
{
	if( query_dofound == &cb_kad_query )	return query_dofound_cb(cb_userptr, cb_kad_query, kad_event);
	if( query_nofound == &cb_kad_query )	return query_nofound_cb(cb_userptr, cb_kad_query, kad_event);
	// NOTE: this point MUST NOT be reached
	DBG_ASSERT( 0 );
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function publish_record
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief do a kad_publish_t on a single record and wait until it is completed
 */
nunit_res_t	kad_publish_testclass_t::test_publish_start(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// build a record for this kad_peer_t
	kad_keyid_t	keyid	= "kad_keyid_t for kad_publish_nunit";
	kad_recid_t	recid	= "kad_recid_t for kad_publish_nunit";
	datum_t		payload	= datum_t("dummy data");
	kad_rec_t	kad_rec = kad_rec_t(recid, keyid, delay_t::from_sec(60), payload);	
	
	// start a kad_store_t
	kad_publish	= nipmem_new kad_publish_t();
	kad_err		= kad_publish->start(kad_peer, kad_rec, this, NULL);
	if( kad_err.failed() )	return NUNIT_RES_ERROR;

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a kad_publish_t has an event to notify
 */
bool	kad_publish_testclass_t::neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_publish_rpc
						, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_DBG("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be publish_ok
	DBG_ASSERT( kad_event.is_publish_ok() );
	
	// notify the nunit_res_t depending on the kad_event
	bool	tokeep;
	if( kad_event.is_completed() )	tokeep	= nunit_ftor(NUNIT_RES_OK);
	else				tokeep	= nunit_ftor(NUNIT_RES_ERROR);
	if( !tokeep )	return false;

	// NOTE: dont delete the kad_publish

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function query_dofound
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief do a kad_query_t on the just published records and expect to find it
 */
nunit_res_t	kad_publish_testclass_t::test_query_dofound(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_keyid_t	keyid	= "kad_keyid_t for kad_publish_nunit";
	kad_err_t	kad_err;
	
	// start a kad_store_t
	query_dofound	= nipmem_new kad_query_t();
	kad_err		= query_dofound->start(kad_peer, keyid, 0, delay_t::from_sec(10), this, NULL);
	if( kad_err.failed() )	return NUNIT_RES_ERROR;

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a query_dofound has an event to notify
 */
bool	kad_publish_testclass_t::query_dofound_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_DBG("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be query_ok
	DBG_ASSERT( kad_event.is_query_ok() );

	// delete the query_dofound and mark it unused
	nipmem_zdelete query_dofound;

	// if the kad_event_t is fatal, report a error to nunit
	if( kad_event.is_fatal() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// sanity check - here the kad_event_t MUST be recdups
	DBG_ASSERT( kad_event.is_recdups() );

	// do an alias for the kad_event_t result
	bool			has_more_record;
	const kad_recdups_t &	kad_recdups	= kad_event.get_recdups(&has_more_record);		
	// test if the result is the expected one
	if( kad_recdups.size() != 1 || kad_recdups[0].get_recid() != "kad_recid_t for kad_publish_nunit"
						|| has_more_record == true ){
		// report faillure
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	// notify success
	nunit_ftor(NUNIT_RES_OK);
	// return tokeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function publish_record
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief do a kad_publish_t on a single record and wait until it is completed
 */
nunit_res_t	kad_publish_testclass_t::test_publish_stop(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// sanity check - the kad_publish_t MUST be initialized
	DBG_ASSERT( kad_publish );
	// delete the kad_publish
	nipmem_zdelete	kad_publish;
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function query_nofound
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief do a kad_query_t on the just published records and expect NOT to find it
 */
nunit_res_t	kad_publish_testclass_t::test_query_nofound(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_keyid_t	keyid	= "kad_keyid_t for kad_publish_nunit";
	kad_err_t	kad_err;
	
	// start a kad_store_t
	query_nofound	= nipmem_new kad_query_t();
	kad_err		= query_nofound->start(kad_peer, keyid, 0, delay_t::from_sec(10), this, NULL);
	if( kad_err.failed() )	return NUNIT_RES_ERROR;

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified when a query_nofound has an event to notify
 */
bool	kad_publish_testclass_t::query_nofound_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event) throw()
{
	// log to debug
	KLOG_DBG("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be query_ok
	DBG_ASSERT( kad_event.is_query_ok() );

	// delete the query_nofound and mark it unused
	nipmem_zdelete query_nofound;

	/* NOTE: 
	* - here some race may happen as there is no way to know if the kad_delete_t is completed
	* - if it not completed, the kad_query_t will found a result
	* - but as in this nunit is done on a single peer and over localhost, the race is very unlikely
	* => the race is unlikely and this is only a nunit. so i leave it as is :)
	*/

	// if the kad_event_t is fatal, report a error to nunit
	if( kad_event.is_fatal() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// sanity check - here the kad_event_t MUST be recdups
	DBG_ASSERT( kad_event.is_recdups() );

	// do an alias for the kad_event_t result
	bool			has_more_record;
	const kad_recdups_t &	kad_recdups	= kad_event.get_recdups(&has_more_record);		
	// test if the result is the expected one
	if( kad_recdups.size() != 0 ){
		// report faillure
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	// notify success
	nunit_ftor(NUNIT_RES_OK);
	// return tokeep
	return false;
}


NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the unit test for the \ref kad_listener_t

\par Brief Description
- closestnode: MUST return all the kad_addr_t of all the kad_peer_t
- store_record : store one record on every kad_peer_t
- query_some_found: query the ALL just storeed record from EACH kad_peer_t
  - MUST found some of them
- query_found: query the ALL just storeed record from EACH kad_peer_t
  - MUST found them all
- delete_record: delete all the record from a given kad_peer_t
- query_some_nofound: query the ALL just storeed record from EACH kad_peer_t
  - MUST NOT found any
- query_nofound: query the ALL just storeed record from EACH kad_peer_t
  - MUST NOT found any

*/

/* system include */
/* local include */
#include "neoip_kad_cmd_nunit.hpp"
#include "neoip_kad_listener.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_event.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_kad_closestnode.hpp"
#include "neoip_kad_store.hpp"
#include "neoip_kad_query_some.hpp"
#include "neoip_kad_query.hpp"
#include "neoip_kad_delete.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_bstrap_profile_t constant
#if 1
	const size_t	kad_cmd_testclass_t::NB_PEER_TO_LAUNCH	= 2;
	const delay_t	kad_cmd_testclass_t::PEER_BSTRAP_DELAY	= delay_t::from_sec(10);
#else
	const size_t	kad_cmd_testclass_t::NB_PEER_TO_LAUNCH	= 5;
	const delay_t	kad_cmd_testclass_t::PEER_BSTRAP_DELAY	= delay_t(delay_t::INFINITE_VAL);
#endif
// end of constants definition

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_cmd_testclass_t::kad_cmd_testclass_t()	throw()
{
	// zero some field
	udp_vresp	= NULL;
	kad_listener	= NULL;
}

/** \brief Destructor
 */
kad_cmd_testclass_t::~kad_cmd_testclass_t()	throw()
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
nunit_err_t	kad_cmd_testclass_t::neoip_nunit_testclass_init()	throw()
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
	kad_realmid_t	realmid	= "kad_cmd_nunit_realm";	// the realm_id for this unittest

	// init all the kad_peer_t
	for(size_t i = 0; i < NB_PEER_TO_LAUNCH; i++){
		kad_peer_t *	kad_peer;
		kad_peerid_t	kad_peerid	= "peerid for peer " + OSTREAMSTR(i);
		// create the kad_peer_t
		kad_peer	= nipmem_new kad_peer_t();
		kad_err		= kad_peer->start(kad_listener, realmid, kad_peerid);
		DBG_ASSERT( kad_err.succeed() );
		// put the created kad_peer_t in peer_db
		peer_db.push_back(kad_peer);
		
		// build a record for this kad_peer_t
		kad_keyid_t	keyid	= std::string("record of peer " + OSTREAMSTR(i)).c_str();
		kad_recid_t	recid	= kad_recid_t::build_random();	
		std::string	data_str= "data for peer " + OSTREAMSTR(i);
		datum_t		payload	= datum_t(data_str.c_str(), data_str.size());
		kad_rec_t	kad_rec = kad_rec_t(recid, keyid, delay_t::from_sec(60), payload);
		// put the kad_rec_t in the record_db
		record_db.push_back(kad_rec);
	}
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	kad_cmd_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// delete all the pending element of closestnode_db
	while( !closestnode_db.empty() ){
		nipmem_delete	*closestnode_db.begin();
		closestnode_db.erase(closestnode_db.begin());
	}
	// delete all the pending element of query_some_nofound_db
	while( !query_some_dofound_db.empty() ){
		nipmem_delete	*query_some_dofound_db.begin();
		query_some_dofound_db.erase(query_some_dofound_db.begin());
	}	
	// delete all the pending element of query_some_nofound_db
	while( !query_some_nofound_db.empty() ){
		nipmem_delete	*query_some_nofound_db.begin();
		query_some_nofound_db.erase(query_some_nofound_db.begin());
	}	
	// delete all the pending element of query_nofound_db
	while( !query_dofound_db.empty() ){
		nipmem_delete	*query_dofound_db.begin();
		query_dofound_db.erase(query_dofound_db.begin());
	}	
	// delete all the pending element of query_nofound_db
	while( !query_nofound_db.empty() ){
		nipmem_delete	*query_nofound_db.begin();
		query_nofound_db.erase(query_nofound_db.begin());
	}	
	// delete all the pending element of delete_db
	while( !delete_db.empty() ){
		nipmem_delete	*delete_db.begin();
		delete_db.erase(delete_db.begin());
	}	
	// delete all the pending element of store_db
	while( !store_db.empty() ){
		nipmem_delete	*store_db.begin();
		store_db.erase(store_db.begin());
	}
	// delete all the pending element of peer_db
	while( !peer_db.empty() ){
		nipmem_delete	*peer_db.begin();
		peer_db.erase(peer_db.begin());
	}
	// delete the kad_listener_t if needed
	nipmem_zdelete	kad_listener;
	// delete the udp_vresp_t if needed
	nipmem_zdelete	udp_vresp;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//               main kad_query_t callback - dispatching to subcallback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_query_t has an event to notify
 */
bool	kad_cmd_testclass_t::neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw()
{
	std::list<kad_query_t *>::iterator	iter;
	
	// if this kad_query_t is from query_dofound_db
	for(iter = query_dofound_db.begin(); iter != query_dofound_db.end() && *iter != &cb_kad_query; iter++);
	if(iter != query_dofound_db.end())	return query_dofound_cb(cb_userptr, cb_kad_query, kad_event);
	
	// if this kad_query_t is from query_nofound_db
	for(iter = query_nofound_db.begin(); iter != query_nofound_db.end() && *iter != &cb_kad_query; iter++);
	if(iter != query_nofound_db.end())	return query_nofound_cb(cb_userptr, cb_kad_query, kad_event);

	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return false;
}

/** \brief callback notified when a kad_query_some_t has an event to notify
 */
bool	kad_cmd_testclass_t::neoip_kad_query_some_cb(void *cb_userptr, kad_query_some_t &cb_kad_query_some
						, const kad_event_t &kad_event)	throw()
{
	std::list<kad_query_some_t *>::iterator	iter;
	
	// if this kad_query_some_t is from query_some_dofound_db
	for(iter = query_some_dofound_db.begin(); iter != query_some_dofound_db.end() && *iter != &cb_kad_query_some; iter++);
	if(iter != query_some_dofound_db.end())	return query_some_dofound_cb(cb_userptr, cb_kad_query_some, kad_event);
	
	// if this kad_query_some_t is from query_some_nofound_db
	for(iter = query_some_nofound_db.begin(); iter != query_some_nofound_db.end() && *iter != &cb_kad_query_some; iter++);
	if(iter != query_some_nofound_db.end())	return query_some_nofound_cb(cb_userptr, cb_kad_query_some, kad_event);

	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to wait_for_btrap
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief wait for the kad_peer_t's to bootstrap with each other
 */
nunit_res_t	kad_cmd_testclass_t::wait_for_bstrap(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// start the bstrap_timeout
	bstrap_timeout.start(PEER_BSTRAP_DELAY, this, NULL);

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback called when the expire_timeout expire
 */
bool	kad_cmd_testclass_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// stop the bstrap_timeout
	bstrap_timeout.stop();
	// notify success
	return nunit_ftor(NUNIT_RES_OK);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to store record
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Publish one record on every kad_peer_t
 */
nunit_res_t	kad_cmd_testclass_t::store_record(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_store_t *	kad_store;
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter");
	
	// store all the kad_rec_t
	for(size_t i = 0; i < NB_PEER_TO_LAUNCH; i++){
		// start a kad_store_t
		kad_store	= nipmem_new kad_store_t();
		kad_err		= kad_store->start(peer_db[i], record_db[i], delay_t::from_sec(10)
							, this, NULL);
		if( kad_err.failed() )	return NUNIT_RES_ERROR;
		// put the kad_store_t in the store_db
		store_db.push_back(kad_store);
	}

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback notified when a kad_store_t has an event to notify
 */
bool	kad_cmd_testclass_t::neoip_kad_store_cb(void *cb_userptr, kad_store_t &cb_kad_store
						, const kad_event_t &kad_event)	throw()
{
	kad_store_t *	kad_store = &cb_kad_store;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be store_ok()
	DBG_ASSERT( kad_event.is_store_ok() );
	
	// delete the kad_store_t and remove it from the database
	store_db.remove(kad_store);
	nipmem_delete	kad_store;
	
	// if the kad_event_t is fatal, report a error to nunit
	if( kad_event.is_fatal() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// if the store_db is now empty, report a sucess to nunit
	if( store_db.empty() ){
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to query_some_dofound
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Publish one record on every kad_peer_t
 */
nunit_res_t	kad_cmd_testclass_t::query_some_dofound(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter");

#if 1	// TODO to reenable - disabled to experiment in debugging what seems a bootstrapping issue
	kad_query_some_t *	kad_query_some;
	// query_some all the kad_rec_t
	for(size_t i = 0; i < NB_PEER_TO_LAUNCH; i++){
		for(size_t j = 0; j < NB_PEER_TO_LAUNCH; j++){
			// start a kad_query_some_t
			kad_query_some	= nipmem_new kad_query_some_t();
			kad_err		= kad_query_some->start(peer_db[i], record_db[j].get_keyid(), 1, 10
							, delay_t::from_sec(10), this, (void *)j);
			if( kad_err.failed() )	return NUNIT_RES_ERROR;
			// put the kad_query_some_t in the query_some_dofound_db
			query_some_dofound_db.push_back(kad_query_some);
		}
	}
#endif
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback notified when a kad_query_some_t has an event to notify
 */
bool	kad_cmd_testclass_t::query_some_dofound_cb(void *cb_userptr, kad_query_some_t &cb_kad_query_some
						, const kad_event_t &kad_event)	throw()
{
	kad_query_some_t  *kad_query_some = &cb_kad_query_some;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be query_some_ok()
	DBG_ASSERT( kad_event.is_query_some_ok() );
	
	// delete the kad_query_some_t and remove it from the database
	query_some_dofound_db.remove(kad_query_some);
	nipmem_delete	kad_query_some;
	
	// if the kad_event_t is fatal, report a error to nunit
	if( kad_event.is_fatal() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// sanity check - here the kad_event_t MUST be recdups
	DBG_ASSERT( kad_event.is_recdups() );

	size_t	peer_idx= (size_t)cb_userptr;
	bool		has_more_record;
	kad_recdups_t	kad_recdups	= kad_event.get_recdups(&has_more_record);
	std::string	data_str	= "data for peer " + OSTREAMSTR(peer_idx);
	datum_t		data		= datum_t(data_str.c_str(), data_str.size());
	// test if the result is the expected one
	// - aka to have exactly one record per getall
	// - this record to have the expected content
	if( kad_recdups.size() != 1 || kad_recdups[0].get_payload() != data ){
		// report faillure
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// if the query_some_dofound_db is now empty, report a sucess to nunit
	if( query_some_dofound_db.empty() ){
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to query_dofound
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Publish one record on every kad_peer_t
 */
nunit_res_t	kad_cmd_testclass_t::query_dofound(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_query_t *	kad_query;
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter");
	
	// query all the kad_rec_t
	for(size_t i = 0; i < NB_PEER_TO_LAUNCH; i++){
		for(size_t j = 0; j < NB_PEER_TO_LAUNCH; j++){
			// start a kad_query_t
			kad_query	= nipmem_new kad_query_t();
			kad_err		= kad_query->start(peer_db[i], record_db[j].get_keyid(), 1
							, delay_t::from_sec(10), this, (void *)j);
			if( kad_err.failed() )	return NUNIT_RES_ERROR;
			// put the kad_query_t in the query_dofound_db
			query_dofound_db.push_back(kad_query);
		}
	}

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback notified when a kad_query_t has an event to notify
 */
bool	kad_cmd_testclass_t::query_dofound_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw()
{
	kad_query_t *	kad_query = &cb_kad_query;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be query_ok()
	DBG_ASSERT( kad_event.is_query_ok() );
	
	// delete the kad_query_t and remove it from the database
	query_dofound_db.remove(kad_query);
	nipmem_delete	kad_query;
	
	// if the kad_event_t is fatal, report a error to nunit
	if( kad_event.is_fatal() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// sanity check - here the kad_event_t MUST be recdups
	DBG_ASSERT( kad_event.is_recdups() );
	
	
	size_t	peer_idx= (size_t)cb_userptr;
	bool		has_more_record;
	kad_recdups_t	kad_recdups	= kad_event.get_recdups(&has_more_record);
	std::string	data_str	= "data for peer " + OSTREAMSTR(peer_idx);
	datum_t		data		= datum_t(data_str.c_str(), data_str.size());
	// test if the result is the expected one
	// - aka to have exactly one record per getall
	// - this record to have the expected content
	if( kad_recdups.size() != 1 || kad_recdups[0].get_payload() != data ){
		// report faillure
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// if the query_dofound_db is now empty, report a sucess to nunit
	if( query_dofound_db.empty() ){
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to delete record
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Publish one record on every kad_peer_t
 */
nunit_res_t	kad_cmd_testclass_t::delete_record(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_delete_t *	kad_delete;
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter");
	
	// delete all the kad_rec_t
	for(size_t i = 0; i < NB_PEER_TO_LAUNCH; i++){
		kad_peer_t *	kad_peer	= peer_db[i];
		kad_rec_t &	kad_rec		= record_db[i];
		// start a kad_delete_t
		kad_delete	= nipmem_new kad_delete_t();
		kad_err		= kad_delete->start(kad_peer, kad_rec.get_recid(), kad_rec.get_keyid()
							, delay_t::from_sec(10), this, NULL);
		if( kad_err.failed() )	return NUNIT_RES_ERROR;
		// put the kad_delete_t in the delete_db
		delete_db.push_back(kad_delete);
	}

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback notified when a kad_delete_t has an event to notify
 */
bool	kad_cmd_testclass_t::neoip_kad_delete_cb(void *cb_userptr, kad_delete_t &cb_kad_delete
						, const kad_event_t &kad_event)	throw()
{
	kad_delete_t *	kad_delete = &cb_kad_delete;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be delete_ok()
	DBG_ASSERT( kad_event.is_delete_ok() );
	
	// delete the kad_delete_t and remove it from the database
	delete_db.remove(kad_delete);
	nipmem_delete	kad_delete;
	
	// if the kad_event_t is fatal, report a error to nunit
	if( kad_event.is_fatal() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// if the delete_db is now empty, report a sucess to nunit
	if( delete_db.empty() ){
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to query_some_nofound
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Publish one record on every kad_peer_t
 */
nunit_res_t	kad_cmd_testclass_t::query_some_nofound(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_query_some_t *	kad_query_some;
	kad_err_t		kad_err;
	// log to debug
	KLOG_DBG("enter");
	
	// query_some all the kad_rec_t
	for(size_t i = 0; i < NB_PEER_TO_LAUNCH; i++){
		for(size_t j = 0; j < NB_PEER_TO_LAUNCH; j++){
			// start a kad_query_some_t
			kad_query_some	= nipmem_new kad_query_some_t();
			kad_err		= kad_query_some->start(peer_db[i], record_db[j].get_keyid()
							, 1, 10
							, delay_t::from_sec(10), this, (void *)j);
			if( kad_err.failed() )	return NUNIT_RES_ERROR;
			// put the kad_query_some_t in the query_some_nofound_db
			query_some_nofound_db.push_back(kad_query_some);
		}
	}

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback notified when a kad_query_some_t has an event to notify
 */
bool	kad_cmd_testclass_t::query_some_nofound_cb(void *cb_userptr, kad_query_some_t &cb_kad_query_some
						, const kad_event_t &kad_event)	throw()
{
	kad_query_some_t *	kad_query_some = &cb_kad_query_some;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be query_some_ok()
	DBG_ASSERT( kad_event.is_query_some_ok() );
	
	// delete the kad_query_some_t and remove it from the database
	query_some_nofound_db.remove(kad_query_some);
	nipmem_delete	kad_query_some;
	
	// if the kad_event_t is fatal, report a error to nunit
	if( kad_event.is_fatal() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// sanity check - here the kad_event_t MUST be recdups
	DBG_ASSERT( kad_event.is_recdups() );
	
	
	bool		has_more_record;
	kad_recdups_t	kad_recdups	= kad_event.get_recdups(&has_more_record);
	// if the returned kad_recdups_t is not empty, notify an error
	if( kad_recdups.size() != 0 ){
		// report faillure
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// if the query_some_nofound_db is now empty, report a sucess to nunit
	if( query_some_nofound_db.empty() ){
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to query_nofound
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Publish one record on every kad_peer_t
 */
nunit_res_t	kad_cmd_testclass_t::query_nofound(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_query_t *	kad_query;
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter");
	
	// query all the kad_rec_t
	for(size_t i = 0; i < NB_PEER_TO_LAUNCH; i++){
		for(size_t j = 0; j < NB_PEER_TO_LAUNCH; j++){
			// start a kad_query_t
			kad_query	= nipmem_new kad_query_t();
			kad_err		= kad_query->start(peer_db[i], record_db[j].get_keyid(), 1
							, delay_t::from_sec(10), this, (void *)j);
			if( kad_err.failed() )	return NUNIT_RES_ERROR;
			// put the kad_query_t in the query_nofound_db
			query_nofound_db.push_back(kad_query);
		}
	}

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback notified when a kad_query_t has an event to notify
 */
bool	kad_cmd_testclass_t::query_nofound_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw()
{
	kad_query_t *	kad_query = &cb_kad_query;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be query_ok()
	DBG_ASSERT( kad_event.is_query_ok() );
	
	// delete the kad_query_t and remove it from the database
	query_nofound_db.remove(kad_query);
	nipmem_delete	kad_query;
	
	// if the kad_event_t is fatal, report a error to nunit
	if( kad_event.is_fatal() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// sanity check - here the kad_event_t MUST be recdups
	DBG_ASSERT( kad_event.is_recdups() );
	
	
	bool		has_more_record;
	kad_recdups_t	kad_recdups	= kad_event.get_recdups(&has_more_record);
	// if the returned kad_recdups_t is not empty, notify an error
	if( kad_recdups.size() != 0 ){
		// report faillure
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// if the query_nofound_db is now empty, report a sucess to nunit
	if( query_nofound_db.empty() ){
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function to closestnode record
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Publish one record on every kad_peer_t
 */
nunit_res_t	kad_cmd_testclass_t::closestnode(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_closestnode_t *	kad_closestnode;
	kad_err_t		kad_err;
	// log to debug
	KLOG_DBG("enter");
	
	// closestnode all the kad_rec_t
	for(size_t i = 0; i < NB_PEER_TO_LAUNCH; i++){
		// start a kad_closestnode_t
		kad_closestnode	= nipmem_new kad_closestnode_t();
		kad_err		= kad_closestnode->start(peer_db[i], kad_targetid_t()
							, NB_PEER_TO_LAUNCH, delay_t::from_sec(10)
							, this, NULL);
		if( kad_err.failed() )	return NUNIT_RES_ERROR;
		// put the kad_closestnode_t in the closestnode_db
		closestnode_db.push_back(kad_closestnode);
	}

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback notified when a kad_closestnode_t has an event to notify
 */
bool	kad_cmd_testclass_t::neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
						, const kad_event_t &kad_event)	throw()
{
	kad_closestnode_t *	kad_closestnode = &cb_kad_closestnode;
	// log to debug
	KLOG_ERR("enter kad_event=" << kad_event);
	// sanity check - the kad_event_t MUST be closestnode_ok()
	DBG_ASSERT( kad_event.is_closestnode_ok() );
	
	// delete the kad_closestnode_t and remove it from the database
	closestnode_db.remove(kad_closestnode);
	nipmem_delete	kad_closestnode;
	
	// if the kad_event_t is fatal, report a error to nunit
	if( kad_event.is_fatal() ){
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}

	// sanity check - the kad_event MUST be is_caddr_arr()
	DBG_ASSERT( kad_event.is_caddr_arr() );
	const kad_caddr_arr_t &kad_caddr_arr	= kad_event.get_caddr_arr();
	
	// the peerid of each launched kad_peer_t MUST be in the kad_caddr_arr
	for(size_t i = 0; i < peer_db.size(); i++ ){
		kad_peer_t *	kad_peer	= peer_db[i];
		size_t		j;
		// check if this kad_peer local peerid is in the kad_caddr_arr
		for(j = 0; j < kad_caddr_arr.size(); j++ ){
			const kad_caddr_t &	kad_caddr = kad_caddr_arr[j];
			if( kad_peer->local_peerid() == kad_caddr.get_peerid() )
				break;
		}
		// if the kad_peer peerid has not been found, report an error
		if( j == kad_caddr_arr.size() ){
			nunit_ftor(NUNIT_RES_ERROR);
			return false;
		}
	}
	
	
	// if the closestnode_db is now empty, report a sucess to nunit
	if( closestnode_db.empty() ){
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END


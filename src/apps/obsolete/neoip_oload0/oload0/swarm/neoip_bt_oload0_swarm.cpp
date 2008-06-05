/*! \file
    \brief Definition of the \ref bt_oload0_swarm_t

\par Brief Description
\ref bt_oload0_swarm_t handles the communication between a bt_swarm_t 
and all the bt_httpo_full_t attached. It is created by the various 
handlers.

- TODO when no more httpo_full_t happen, do a little timeout before deleting
  - idle_timeout
  - thus if a connection goes down and then up, all the connection and data are 
    not discarded
- TODO who to set the profile
  - this object is created by the handler itor
  - so it is up to it to set the profile
  - does different handler have different bt_oload0_swarm_t itor ?
    - if yes, put it in the handler profile
    - if no, put it in the bt_oload0_profile_t

*/

/* system include */
/* local include */
#include "neoip_bt_oload0_swarm.hpp"
#include "neoip_bt_oload0.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"
#include "neoip_bt_swarm_pdeletor.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_event.hpp"
#include "neoip_bt_swarm_peersrc.hpp"
#include "neoip_bt_mfile_helper.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_io_pfile.hpp"
#include "neoip_bt_peersrc_kad.hpp"
#include "neoip_bt_peersrc_http.hpp"
#include "neoip_mimediag.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
bt_oload0_swarm_t::bt_oload0_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	bt_oload0	= NULL;
	bt_io_pfile	= NULL;
	bt_swarm	= NULL;
	peersrc_kad	= NULL;
	peersrc_http	= NULL;
	http_ecnx_pool	= NULL;	
	swarm_pdeletor	= NULL;
}

/** \brief Destructor
 */
bt_oload0_swarm_t::~bt_oload0_swarm_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_oload0_t
	if( bt_oload0 )	bt_oload0->oload0_swarm_unlink(this);
	// delete the httpo_full_db if needed
	while( !httpo_full_db.empty() ){
		nipmem_delete	httpo_full_db.front();
		httpo_full_db.pop_front();
	}
	// delete the bt_swarm_pdeletor_t if needed
	nipmem_zdelete	swarm_pdeletor;
	// delete the bt_http_ecnx_pool_t if needed
	nipmem_zdelete	http_ecnx_pool;
	// delete the bt_peersrc_http_t if needed	
	nipmem_zdelete	peersrc_http;
	// delete the bt_peersrc_kad_t if needed	
	nipmem_zdelete	peersrc_kad;
	// delete the bt_swarm_t if needed
	nipmem_zdelete	bt_swarm;
	// delete the bt_io_pfile_t if needed
	nipmem_zdelete	bt_io_pfile;
}

/** \brief autodelete this object, and return false for convenience
 */
bool	bt_oload0_swarm_t::autodelete()			throw()
{
	// autodelete this 
	nipmem_delete	this;
	// return false
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_oload0_swarm_t &bt_oload0_swarm_t::set_profile(const bt_oload0_swarm_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 * 
 * - NOTE: the httpo_full_db ownership is transfered to this object
 */
bt_err_t bt_oload0_swarm_t::start(bt_oload0_t *bt_oload0, const http_uri_t &nested_uri
					, const bt_mfile_t &bt_mfile
					, const std::list<bt_httpo_full_t *> httpo_full_db)	throw()
{
	// log to debug
	KLOG_WARN("enter innner_uri=" << nested_uri);
	// copy the parameter
	this->bt_oload0		= bt_oload0;
	this->nested_uri	= nested_uri;

	// sanity check - bt_oload0_swarm_t MUST NOT for this nested_uri
	DBG_ASSERT( !bt_oload0->swarm_by_nested_uri(nested_uri) );	
	// link this object to the bt_oload0_t
	bt_oload0->oload0_swarm_dolink(this);

	// init the bt_swarm_t with the bt_mfile from the parameter
	bt_err_t	bt_err;
	bt_err		= build_bt_swarm(bt_mfile);
	if( bt_err.failed() ){
		// delete all the httpo_full_db
		// - as they are not yet locally linker but still owned
		std::list<bt_httpo_full_t *>::const_iterator	iter;
		for(iter = httpo_full_db.begin(); iter != httpo_full_db.end(); iter++)
			nipmem_delete *iter;
		// report the bt_err
		return bt_err;
	}

	// add all the bt_httpo_full_t from the httpo_full_db passed in parameter
	std::list<bt_httpo_full_t *>::const_iterator	iter;
	for(iter = httpo_full_db.begin(); iter != httpo_full_db.end(); iter++)	add_httpo_full( *iter );

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init all the structure for the bt_swarm_t based on the bt_mfile_t from the caller
 */
bt_err_t	bt_oload0_swarm_t::build_bt_swarm(const bt_mfile_t &bt_mfile)	throw()
{
	bt_session_t *	bt_session	= bt_oload0->get_bt_session();
	bt_err_t	bt_err;
	// sanity check - the bt_swarm_t MUST BE null
	DBG_ASSERT( !bt_swarm );	

	// init the bt_io_pfile_t
	bt_io_pfile	= nipmem_new bt_io_pfile_t();
	bt_err		= bt_io_pfile->start(bt_mfile, bt_oload0->io_pfile_dirpath());
	if( bt_err.failed() )	return bt_err;

	// create the bt_swarm_profile_t
	bt_swarm_profile_t	swarm_profile;
	swarm_profile.pselect_policy( bt_pselect_policy_t::SLIDE );

	// start a bt_swarm_t
	bt_swarm	= nipmem_new bt_swarm_t();
	bt_err		= bt_swarm->set_profile(swarm_profile).start(bt_session, bt_mfile
							, bt_io_pfile, this, NULL);
	if( bt_err.failed() )	return bt_err;

	// init the bt_peersrc_kad_t 
	peersrc_kad	= nipmem_new bt_peersrc_kad_t();
	bt_err		= peersrc_kad->start(bt_swarm, bt_oload0->get_kad_peer()
						, bt_swarm->get_swarm_peersrc(), NULL);
	if( bt_err.failed() )	return bt_err;

	// init the bt_peersrc_http_t if there is a announce_uri() in the bt_mfile_t
	if( !bt_mfile.announce_uri().is_null() ){
		peersrc_http	= nipmem_new bt_peersrc_http_t();
		bt_err		= peersrc_http->start(bt_swarm, bt_swarm->get_swarm_peersrc(), NULL);
		if( bt_err.failed() )	return bt_err;
	}
	
	// init the bt_http_ecnx_pool_t
	http_ecnx_pool	= nipmem_new bt_http_ecnx_pool_t();
	bt_err		= http_ecnx_pool->start(bt_swarm);
	if( bt_err.failed() )	return bt_err;

	// init the bt_swarm_pdeletor_t
	// TODO maybe to change the bt_swarm_pdeletor_policy_t with a _arg_t or something
	// - the point is to get something more flexible that a single parameter
	swarm_pdeletor	= nipmem_new bt_swarm_pdeletor_t();
	bt_err		= swarm_pdeletor->start(bt_swarm_pdeletor_policy_t::ALL_NOTREQUIRED, bt_swarm);
	if( bt_err.failed() )	return bt_err;
	
	// return noerror
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a new bt_httpo_full_t to this object
 * 
 * - NOTE: the bt_httpo_full_t ownership is transfered to this object
 */
void	bt_oload0_swarm_t::add_httpo_full(bt_httpo_full_t *httpo_full)	throw()
{
	// sanity check - the bt_swarm_t MUST be already initialized
	DBG_ASSERT( bt_swarm );
	// build the http_rephd_t to reply
	const http_reqhd_t &	http_reqhd	= httpo_full->get_http_reqhd();
	file_range_t		req_range	= http_reqhd.range();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	file_size_t		totfile_size	= bt_mfile.totfile_size();
	const bt_mfile_subfile_t&mfile_subfile	= bt_mfile.subfile_arr()[0];
	file_range_t		rep_range;
	http_rephd_t		http_rephd;
	
	// sanity check - the bt_mfile MUST have a single subfile
	DBG_ASSERT( bt_mfile.subfile_arr().size() == 1 );

	// handle the case differently depending on presence of req_range in the http_reqhd_t
	if( req_range.is_null() ){
		// compute the range to reply
		rep_range	= file_range_t(0, totfile_size-1);
		// build the reply header
		http_rephd.version(http_version_t::V1_1).status_code(200).reason_phrase("OK");
		http_rephd.content_length(rep_range.len());
	}else{
		// compute the range to reply
		rep_range	= req_range;
		rep_range.clamped_by( file_range_t(0, totfile_size-1) );
		// TODO what if the req_range is out of range ?
		// - this case should be tested before ?
		// - this is clearly an error case
		// - should be passed to bt_oload0_herr_t
		DBG_ASSERT(!rep_range.is_null());
		// build the http_rephd_t
		http_rephd.version(http_version_t::V1_1).status_code(206).reason_phrase("Partial Content");
		http_rephd.content_range(rep_range, totfile_size);
	}
	// notify the http client that this server accept RANGE request
	http_rephd.accept_ranges(true);
	// add the mimetype of the content - rfc2616.14.17
	std::string	mimetype_str	= mimediag_t::from_file_path(mfile_subfile.mfile_path());
	if( !mimetype_str.empty() )	http_rephd.header_db().update("Content-Type", mimetype_str);

	// log to debug
	KLOG_WARN("http_rephd=" << http_rephd);

	// start this bt_httpo_full_t
	bt_err_t bt_err	= httpo_full->start(bt_swarm, http_rephd, rep_range, this, NULL);
	if( bt_err.failed() ){
		// log the event 
		KLOG_INFO("httpo_full failed to start due to " << bt_err);
		// delete the httpo_full
		nipmem_zdelete httpo_full;
		return;
	}
	// link this bt_httpo_full_t to the httpo_full_db
	httpo_full_db.push_back(httpo_full);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bt_httpo_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool	bt_oload0_swarm_t::neoip_bt_httpo_full_cb(void *cb_userptr, bt_httpo_full_t &cb_bt_httpo_full
					, const bt_httpo_event_t &httpo_event)	throw()
{
	bt_httpo_full_t*httpo_full	= &cb_bt_httpo_full;
	// log to debug
	KLOG_WARN("enter httpo_event=" << httpo_event);
	// sanity check - bt_httpo_event_t MUST be is_full_ok()
	DBG_ASSERT( httpo_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( httpo_event.get_value() ){
	case bt_httpo_event_t::CNX_CLOSED:
			// remove it from the httpo_full_db
			httpo_full_db.remove(httpo_full);
			// delete the bt_httpo_full_t object itself
			nipmem_zdelete httpo_full;
			// if httpo_full_db is now empty, autodelete
			if( httpo_full_db.empty() )	return autodelete();
			// return dontkeep - as the httpo_full has just been deleted
			return false;
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_swarm_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_swarm_t when to notify an event
 */
bool 	bt_oload0_swarm_t::neoip_bt_swarm_cb(void *cb_userptr, bt_swarm_t &cb_bt_swarm
					, const bt_swarm_event_t &swarm_event)	throw()
{
	// log to debug
	KLOG_WARN("enter event=" << swarm_event);
	// handle the fatal bt_swarm_event_t
	if( swarm_event.is_fatal() ){
		// log the event
		KLOG_INFO("Closing the bt_swarm_t for " << nested_uri << " due to " << swarm_event);
		// autodelete
		return autodelete();
	}
	
	// handle the bt_swarm_event_t according to its value
	switch(swarm_event.get_value()){
	case bt_swarm_event_t::PIECE_NEWLY_AVAIL:{
			std::list<bt_httpo_full_t *>::iterator	iter;
			// notify this new data to each bt_httpo_full_t within the httpo_full_db
			for(iter = httpo_full_db.begin(); iter != httpo_full_db.end(); iter++){
				bt_httpo_full_t *httpo_full	= *iter;
				// notify this bt_httpo_full_t of the new data
				httpo_full->notify_newly_avail_piece();
			}
			break;}
	default:	DBG_ASSERT( 0 );
	}	
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END;





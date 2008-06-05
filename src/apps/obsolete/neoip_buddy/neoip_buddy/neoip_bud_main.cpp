/*! \file
    \brief Definition of the \ref bud_main_t class

*/

/* system include */
#include <iostream>
#include <iomanip>
/* local include */
#include "neoip_bud_main.hpp"
#include "neoip_bud_lshare.hpp"
#include "neoip_bud_http_search.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_http_fdir.hpp"
#include "neoip_kad.hpp"
#include "neoip_file_path.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bud_main_t::bud_main_t()	throw()
{
	// zero the kad_listener pointer
	kad_listener	= NULL;
	kad_peer	= NULL;
}

/** \brief Destructor
 */
bud_main_t::~bud_main_t()	throw()
{
	// delete the http handler
	// - TODO this is shit and has poor error management. what if the start() has not been done
	lib_session_get()->get_httpd()->handler_del("/search", this, NULL);
	// delete all pending http_search_t
	while( !http_search_db.empty() )	nipmem_delete	http_search_db.front();	
	// deinit publish
	publish_deinit();
	// deinit kad
	kad_deinit();
	// deinit the kad_lshare_t
	lshare_deinit();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bool	bud_main_t::start()		throw()
{
	bool	failed;
	// init lshare
	failed = lshare_init();
	if( failed )	goto error;
	// init kad
	failed = kad_init();
	if( failed )	goto deinit_lshare;
	// init publish
	failed = publish_init();
	if( failed )	goto deinit_kad;
	
	// init the http handler
	// - TODO this is shit
	lib_session_get()->get_httpd()->handler_add("/search", this, NULL);

	// return no error
	return false;

deinit_kad:;	kad_deinit();
deinit_lshare:;	lshare_deinit();
error:;		return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 */
httpd_err_t bud_main_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)
										throw()
{
	// log to debug
	KLOG_DBG("request method=" << request.get_method() << " path=" << request.get_path() );

	// intercept the xmlrpc
	if( request.get_method() == "GET" && request.get_path() == "/search"){
		// create a http_search_t - it link itself to bud_main_t
		http_search_t *	http_search	= nipmem_new http_search_t(this);
		// Start the http_search
		return http_search->start(request);
	}

	// if this point is reached, return httpd_err_t::NOT_FOUND
	return httpd_err_t::NOT_FOUND;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    kad_init/term
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief init the kad's peer
 * 
 * @return true on error, false otherwise
 */
bool	bud_main_t::kad_init()	throw()
{
	kad_profile_t	kad_profile	= kad_profile_t::build_default();
	const property_t &session_prop	= *(lib_session_get()->get_property());
	kad_err_t	kad_err;
	inet_err_t	inet_err;

// get parameter from the sessions property
	ipport_addr_t	listen_addr	= session_prop.find_string("kad_listen_addr");
	ipport_addr_t	langrp_addr	= session_prop.find_string("kad_langrp_addr", "255.255.255.255:7777");
	// check that the listen address is not null
	if( listen_addr.is_null() ){
		kad_err = kad_err_t(kad_err_t::ERROR,"The session property kad_listen_addr is NOT set");
		goto error;
	}
	
	// init the udp_vresp_t for the kad_listener
	udp_vresp	= nipmem_new udp_vresp_t();
	inet_err	= udp_vresp->start(listen_addr);
	if( inet_err.failed() )	goto error;

// create the kad_listener_t
	kad_listener	= nipmem_new kad_listener_t();
	// setup the kad_listener_t's listening address
	kad_err		= kad_listener->set_listen_addr(udp_vresp, listen_addr);
	if( kad_err.failed() )	goto error;
	// setup the kad_listener_t's langrp addr
	kad_err		= kad_listener->set_langrp_addr(langrp_addr);
	if( kad_err.failed() )	goto error;
	// start it now
	kad_err		= kad_listener->start();
	if( kad_err.failed() )	goto error;

	// create and start a kad_peer_t
	kad_realmid	= "neoip_buddy realm";
	kad_peer	= nipmem_new kad_peer_t();
	kad_err		= kad_peer->set_profile(kad_profile).start(kad_listener
					, kad_realmid, kad_peerid_t::build_random());
	if( kad_err.failed() )	goto error;

	// return no error
	return false;
	
	
error:	// if needed, delete the kad_listener, mark it unused and return the error
	nipmem_zdelete kad_peer;
	nipmem_zdelete kad_listener;
	nipmem_zdelete udp_vresp;
	// log the event
	KLOG_ERR("Cant init bud_main_t dht due to " << kad_err);
	// return an error
	return true;
}

/** \brief deinit the kad's peer
 */
void	bud_main_t::kad_deinit()	throw()
{
	// delete the kad_peer_t if needed
	nipmem_zdelete	kad_peer;
	// delete the kad_listener if needed
	nipmem_zdelete	kad_listener;
	// delete udp_vresp_t if needed
	nipmem_zdelete	udp_vresp;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    lshare_init/term
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief init the bud_lshare_t
 * 
 * @return true on error, false otherwise
 */
bool	bud_main_t::lshare_init()	throw()
{
	// compute the dircfg_path
	file_path_t	dircfg_path = lib_session_get()->conf_rootdir();
	dircfg_path	/= "buddy_lshare";
	// read the bud_lshare_db_t from this directory	
	bud_lshare_db	= bud_lshare_t::read_from_directory(dircfg_path);

	// init a http_fdir_t for each of the kad_lshare_t
	bud_lshare_db_t::iterator	iter;
	for( iter = bud_lshare_db.begin(); iter != bud_lshare_db.end(); iter++ ){
		const bud_lshare_t &	bud_lshare = iter->second;
		// init the http_fdir_t
		http_fdir_t *	http_fdir = nipmem_new http_fdir_t();
		// set the user_db for the http_fdir_t
		http_fdir->set_user_db( bud_lshare.user_db() );
		// start the http_fdir_t
		bool failed	= http_fdir->start("/buddy/share/" + bud_lshare.local_id()
						, bud_lshare.file_rootpath(), bud_lshare.local_id() );
		// if it fails, log the event and return an error now
		if( failed ){
			KLOG_ERR("Cant init http_fdir_t with local_id=" << bud_lshare.local_id() );
			goto error;
		}
		
		// put http_fdir_t in the database
		http_fdir_db.push_back(http_fdir);
	}

	// return no error
	return false;

error:	// remove the already init http_fdir_t
	lshare_deinit();
	// return an error
	return true;
}

/** \brief deinit the bud_lshare_t
 */
void	bud_main_t::lshare_deinit()	throw()
{
	// delete all the http_fdir_t
	while( !http_fdir_db.empty() ){
		http_fdir_t *	http_fdir = http_fdir_db.front();
		// remove the object from the list
		http_fdir_db.erase( http_fdir_db.begin() );
		// delete the object
		nipmem_delete http_fdir;
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    publish_init/term
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief init the publication of lshare keywords
 * 
 * @return true on error, false otherwise
 */
bool	bud_main_t::publish_init()	throw()
{
	ipport_addr_t	listen_addr	= lib_session_get()->get_httpd()->get_listen_addr();
	std::string	url_prefix	= std::string("http://") + listen_addr.to_string() + "/buddy/share/";
	kad_rec_t	kad_rec;
	kad_err_t	kad_err;
	// init a http_fdir_t for each of the kad_lshare_t
	bud_lshare_db_t::iterator	iter;
	for( iter = bud_lshare_db.begin(); iter != bud_lshare_db.end(); iter++ ){
		const bud_lshare_t &		bud_lshare	= iter->second;
		const std::vector<std::string> &keyword_db	= bud_lshare.keyword_db();

		for( size_t i = 0; i < keyword_db.size(); i++ ){
			kad_store_t *	kad_store;
			// log to debug
			KLOG_DBG("publish record for lshare " << bud_lshare.local_id()
						<< " with keyword " << keyword_db[i]);

			// build the record to publish
			std::string	tmp = url_prefix + bud_lshare.local_id();
			kad_rec = kad_rec_t(keyword_db[i].c_str(), datum_t(tmp.c_str(), tmp.size())
							, delay_t::from_min(10)
							, kad_recid_t::build_random() );
			// launch a publish command for this record without waiting for the result
			kad_store	= nipmem_new kad_store_t();
			kad_err		= kad_store->start(kad_peer, kad_rec, delay_t::NEVER, NULL, NULL);
			DBG_ASSERT( kad_err.succeed() );	// TODO poor error management
			
			// backup the recid, keyid for this record to be able to delete it later
			reckeyid_db.push_back(std::make_pair(kad_rec.get_recid(), kad_rec.get_keyid()));
		}
	}
	
	// return no error
	return false;
}

/** \brief deinit the publication of the lshare keywords
 */
void	bud_main_t::publish_deinit()	throw()
{
#if 0
/* TODO there is a race condition in kad deletion
 * - if the start_delete are done just before the kad_listener delete, the stuff core
 *   due in rpccli_t still being triggered afterward
 */
	// delete all the published records
	while( !reckeyid_db.empty() ){
		std::pair<kad_recid_t, kad_keyid_t> &	tmp = reckeyid_db.front();
		// launch a delete operation on this record without waiting for the result
		kad_listener->start_delete(tmp.first, tmp.second, kad_peer_slotid, delay_t::from_sec(30)
						, NULL, NULL, NULL);
		// remove the object from the list
		reckeyid_db.erase( reckeyid_db.begin() );
	}
#endif
}


NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the \ref bt_oload0_torr_itor_t


\Par Brief Description
- it handle the itor part for the bt_oload0_tort_t
  -# it download the torrent at the inner_uri
  -# it create a bt_mfile_t base on it
  -# it create the bt_oload0_swarm_t for it

\par Limitation
- it works only for single file torrent. all multifile torrent will reply an error
  in all the attached bt_httpo_full_t.

\par Possible Improvement - access multi file .torrent
- to make a way to access multi file .torrent
  - e.g. the inner file may be provided in the inner_uri
  - found out a good scheme for this
  - a variable ? 
  - part of the path ?
  - the path will be good thus the uri will look like pointing at the file, 
    with the proper file extension and stuff
  - NOTE: one could use this for an easy fuse integration
    - the fuse would simply points at the bt_oload0_t
    - maybe some unmodified fuse would do it ?

*/

/* system include */
/* local include */
#include "neoip_bt_oload0_torr_itor.hpp"
#include "neoip_bt_oload0_torr.hpp"
#include "neoip_bt_oload0_swarm.hpp"
#include "neoip_bt_oload0_herr.hpp"
#include "neoip_bt_oload0.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_nested_uri.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_log.hpp"

#include "neoip_bt_http_ecnx_pool.hpp"		// to pass http_sclient_t cnx to bt_http_ecnx_pool_t
#include "neoip_http_client_pool_cnx.hpp"	// to pass http_sclient_t cnx to bt_http_ecnx_pool_t

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
bt_oload0_torr_itor_t::bt_oload0_torr_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	oload0_torr	= NULL;
	http_sclient	= NULL;
}

/** \brief Destructor
 */
bt_oload0_torr_itor_t::~bt_oload0_torr_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_oload0_torr_t
	if( oload0_torr )	oload0_torr->stat_itor_unlink(this);
	// delete the httpo_full_db if needed
	while( !httpo_full_db.empty() ){
		nipmem_delete	httpo_full_db.front();
		httpo_full_db.pop_front();
	}
	// delete the http_sclient_t if needed
	nipmem_zdelete	http_sclient;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t bt_oload0_torr_itor_t::start(bt_oload0_torr_t *oload0_torr, const http_uri_t &nested_uri
						, bt_httpo_full_t *httpo_full)	throw()
{
	const bt_oload0_torr_profile_t &	profile		= oload0_torr->get_profile();
	http_uri_t			inner_uri	= http_nested_uri_t::parse_inner(nested_uri);	
	http_err_t			http_err;
	
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->oload0_torr	= oload0_torr;
	this->nested_uri	= nested_uri;
	// link this object to the bt_oload0_torr_t
	oload0_torr->stat_itor_dolink(this);

	// add this bt_httpo_full_t
	add_httpo_full(httpo_full);

	// launch the http_client_t to get the .torrent file from the inner_uri
	http_sclient	= nipmem_new http_sclient_t();
	http_err	= http_sclient->set_profile(profile.http_sclient()).start(inner_uri, this, NULL);
	if( http_err.failed() )	return bt_err_from_http(http_err);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a new bt_httpo_full_t to this object
 * 
 * - this is called during the life of bt_oload0_too_itor_t in case a new 
 *   bt_httpo_full_t matches the nested_uri
 */
void	bt_oload0_torr_itor_t::add_httpo_full(bt_httpo_full_t *httpo_full)	throw()
{
	const http_reqhd_t &	http_reqhd	= httpo_full->get_http_reqhd();
	const http_uri_t &	nested_uri	= http_reqhd.uri();
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the bt_httpo_full_t uri MUST be the same as the local nested_uri
	DBG_ASSERT( http_reqhd.uri() == nested_uri );
	// add this httpo_full_t to the httpo_full_db
	httpo_full_db.push_back( httpo_full );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	bt_oload0_torr_itor_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &sclient_res)	throw() 
{
	bt_oload0_t *		bt_oload0	= oload0_torr->bt_oload0;
	bt_oload0_swarm_t *	oload0_swarm;
	bt_err_t		bt_err;
	// log to debug
	KLOG_WARN("enter http_sclient_res="		<< sclient_res);
	KLOG_WARN("sclient_res.full_get_ok()="	<< sclient_res.full_get_ok());
	
	// if there is an error in the http_sclient_t, it is an error
	if( sclient_res.cnx_err().failed() )	return handle_itor_error("Inner HTTP error");
	if( !sclient_res.full_get_ok() )	return handle_itor_error("Inner HTTP error");
	
	// build the bt_mfile_t for this bt_oload0_torr_itor_t
	bt_mfile_t	bt_mfile;
	bt_mfile	= bt_mfile_t::from_bencode(sclient_res.reply_body().to_datum(datum_t::NOCOPY));
	// if the bt_mfile_t convertion is not possible, it is an error
	if( bt_mfile.is_null() )	return handle_itor_error("The inner uri is not a .torrent");
	// complete the initialization fo the bt_mfile
	bt_mfile.complete_init();
	
	// if it is not a single-file torrent, it is an error
	// - TODO it may be changed later, if the inner_uri specify a given subfile
	if( bt_mfile.subfile_arr().size() != 1 )
		return handle_itor_error("The torrent contains multiple files. it is not supported");

	// steal the httpo_full_db to pass its ownership to the bt_oload0_swarm_t
	std::list<bt_httpo_full_t *>	stolen_httpo_full_db	= httpo_full_db;
	httpo_full_db	= std::list<bt_httpo_full_t *>();
	
	// create and start the bt_oload0_swarm_t for this nested_uri
	oload0_swarm	= nipmem_new bt_oload0_swarm_t();
	bt_err		= oload0_swarm->start(bt_oload0, nested_uri, bt_mfile, stolen_httpo_full_db);
	if( bt_err.failed() ){
		// log the error
		KLOG_INFO("Failed to launch the bt_swarm_t for " << nested_uri << " due to " << bt_err);
		// delete the just created bt_oload0_swarm_t
		nipmem_zdelete	oload0_swarm;
		// autodelete - no handle_itor_error() as all the httpo_full_db are now owned by bt_oload0_swarm_t
		nipmem_delete	this;
		return false;
	}
#if 0	// TODO this seems to trigger a bug for some unknown reasons
	// - the same works ok on bt_oload0_stat_itor_t
	
	// put the http_sclient connection to the http_client_pool_t of the bt_http_ecnx_t
	// - NOTE: it may be reused by bt_http_ecnx_pool_t and this connection
	//   establishement of the original connection wont be wasted.
	bt_http_ecnx_pool_t *http_ecnx_pool	= oload0_swarm->get_http_ecnx_pool();
	http_sclient->forward_cnx_to_pool(http_ecnx_pool->get_http_client_pool());
#endif
	// autodelete
	nipmem_delete	this;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      handle_itor_error
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief handle an error in the itor connection
 * 
 * - NOTE: it delete the object itself
 * 
 * @return false for the convenience to simulate a dontkeep for the bt_oload0_torr_itor_t
 */
bool	bt_oload0_torr_itor_t::handle_itor_error(const std::string &reason)	throw()
{
	bt_oload0_t *		bt_oload0	= oload0_torr->bt_oload0;
	bt_oload0_herr_t *	oload0_herr	= bt_oload0->get_oload0_herr();
	http_rephd_t		http_rephd;
	// build the http_rephd to reply in case of error
	http_rephd.status_code(404).reason_phrase(reason);

	// pass all the httpo_full_t to the bt_oload0_herr
	// - to hold them during the http_rephd_t send
	while( !httpo_full_db.empty() ){
		bt_httpo_full_t * httpo_full	= httpo_full_db.front();
		// remove this httpo_full from httpo_full_db
		httpo_full_db.pop_front();	
		// pass it to the oload0_herr
		oload0_herr->add_httpo_full(httpo_full, http_rephd);
	}
	// autodelete
	nipmem_delete	this;
	return false;	
}


NEOIP_NAMESPACE_END;





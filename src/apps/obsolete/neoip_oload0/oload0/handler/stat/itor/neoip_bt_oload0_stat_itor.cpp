/*! \file
    \brief Definition of the \ref bt_oload0_stat_itor_t

\par About the file size
- if it is provided as variable of the outter_uri, use it directly
- if it is not provided, do a getlen_sclient http_sclient_t JUST to get the file size.

*/

/* system include */
/* local include */
#include "neoip_bt_oload0_stat_itor.hpp"
#include "neoip_bt_oload0_stat.hpp"
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
bt_oload0_stat_itor_t::bt_oload0_stat_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	oload0_stat	= NULL;
	getlen_sclient	= NULL;
}

/** \brief Destructor
 */
bt_oload0_stat_itor_t::~bt_oload0_stat_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_oload0_stat_t
	if( oload0_stat )	oload0_stat->stat_itor_unlink(this);
	// delete the httpo_full_db if needed
	while( !httpo_full_db.empty() ){
		nipmem_delete	httpo_full_db.front();
		httpo_full_db.pop_front();
	}
	// delete the http_sclient_t if needed
	nipmem_zdelete	getlen_sclient;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t bt_oload0_stat_itor_t::start(bt_oload0_stat_t *oload0_stat, const http_uri_t &nested_uri
						, bt_httpo_full_t *httpo_full)	throw()
{
	http_uri_t	outter_uri	= http_nested_uri_t::parse_outter(nested_uri);	
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->oload0_stat	= oload0_stat;
	this->nested_uri	= nested_uri;
	// link this object to the bt_oload0_stat_t
	oload0_stat->stat_itor_dolink(this);

	// add this bt_httpo_full_t
	add_httpo_full(httpo_full);
		
	// if the outter_uri DO contain a variable filelen, launch a zero timer to launch bt_oload0_swarm_t
	if( outter_uri.var().contain_key("filelen") ){
		filelen_zerotimer.append(this, NULL);
		return bt_err_t::OK;
	}
	
	// launch the http_client_t to get the length of the data
	// - if not yet provided in the outter_uri variable "filelen"
	// - TODO this use a lame range...it should for the HEAD
	//   stuff which is the commonly used stuff for that
	http_uri_t	inner_uri	= http_nested_uri_t::parse_inner(nested_uri);	
	http_reqhd_t	http_reqhd	= http_reqhd_t().uri(inner_uri).range(file_range_t(1,1));
	http_err_t	http_err;
	getlen_sclient	= nipmem_new http_sclient_t();
	http_err	= getlen_sclient->start(http_reqhd, this, NULL);
	if( http_err.failed() )	return bt_err_from_http(http_err);

	// return no error
	return bt_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	bt_oload0_stat_itor_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	http_uri_t	outter_uri	= http_nested_uri_t::parse_outter(nested_uri);
	// sanity check - the outter_uri MUST contains the "filelen" variable
	DBG_ASSERT( outter_uri.var().contain_key("filelen") );
	// sanity check - the http_sclient_t MUST NOT be initialized
	DBG_ASSERT( !getlen_sclient );
	
	// get the filelen from the outter_uri variable
	file_size_t	totfile_len	= atoi(outter_uri.var().get_first_value("filelen").c_str());
	// launch the bt_oload0_swarm_t
	launch_oload0_swarm(totfile_len);

	// autodelete
	nipmem_delete	this;
	return false;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a new bt_httpo_full_t to this object
 */
void	bt_oload0_stat_itor_t::add_httpo_full(bt_httpo_full_t *httpo_full)	throw()
{
	const http_reqhd_t &	http_reqhd	= httpo_full->get_http_reqhd();
	// log to debug
	KLOG_WARN("enter");
	// sanity check - the bt_httpo_full_t uri MUST be the same as the local nested_uri
	DBG_ASSERT( nested_uri == http_reqhd.uri() );
	// add this httpo_full_t to the httpo_full_db
	httpo_full_db.push_back( httpo_full );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the bt_oload0_swarm_t based on the totfile_len
 * 
 * - it is required to return the bt_oload0_swarm_t to put back the original 
 *   getlen_sclient in the http_client_pool_t in case it has been used
 *   to discover the totfile_len.
 * 
 * @return a pointer on bt_oload0_swarm_t, NULL if an error occured
 */
bt_oload0_swarm_t *	bt_oload0_stat_itor_t::launch_oload0_swarm(const file_size_t &totfile_len) throw()
{
	bt_oload0_t *		bt_oload0	= oload0_stat->bt_oload0;
	bt_mfile_t		bt_mfile	= build_bt_mfile(totfile_len);
	bt_oload0_swarm_t *	oload0_swarm;
	bt_err_t		bt_err;
	// log to debug
	KLOG_WARN("enter totfile_len=" << totfile_len);	

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
	}
	// return a pointer on bt_oload0_swarm_t
	return oload0_swarm;
}

/** \brief Build the bt_mfile_t for this connection
 * 
 * - TODO may be ported on top of bt_mfile_helper_t::from_http_uri()
 */
bt_mfile_t	bt_oload0_stat_itor_t::build_bt_mfile(const file_size_t &totfile_len)	const throw()
{
	const bt_oload0_stat_profile_t &	profile		= oload0_stat->get_profile();
	http_uri_t			inner_uri	= http_nested_uri_t::parse_inner(nested_uri);	
	bt_mfile_t			bt_mfile;
	// set the bt_mfile.name() - not required but nice to have in wikidbg :)
	bt_mfile.name		( inner_uri.to_string() );

	// set the piece_len - with a quite short piece_len to improve the sharing
	bt_mfile.piecelen	( profile.mfile_piecelen() );

	// derive the infohash from the inner_uri
	bt_mfile.infohash	( profile.infohash_prefix_str() + inner_uri.to_string() );

	// build the bt_mfile_subfile_t with the inner_uri
	bt_mfile_subfile_t	mfile_subfile;
	mfile_subfile		= bt_mfile_subfile_t(inner_uri.path().basename(), totfile_len);
	mfile_subfile.uri_arr()	+= inner_uri;
	// add this bt_mfile_subfile_t to the bt_mfile.subfile_arr
	bt_mfile.subfile_arr()	+= mfile_subfile;

	// set the bt_mfile.do_piecehash to false as piecehash_arr is empty
	bt_mfile.do_piecehash(false);
	
	// complete the bt_mfile_t initialization
	bt_mfile.complete_init();	

	// return the just built bt_mfile
	return bt_mfile;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	bt_oload0_stat_itor_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &sclient_res)	throw() 
{
	const http_rephd_t &	http_rephd	= sclient_res.http_rephd();
	bt_err_t		bt_err;
	// log to debug
	KLOG_WARN("enter http_sclient_res=" << sclient_res);
	KLOG_WARN("sclient_res.part_get_ok()=" << sclient_res.part_get_ok());
	
	// if there is an error in the http_sclient_t, it is an error
	if( sclient_res.cnx_err().failed() )	return handle_itor_error("Inner HTTP error");
	if( !sclient_res.part_get_ok() )	return handle_itor_error("Inner HTTP error");

	// get the totfile_len from the http_rephd
	file_size_t	totfile_len;
	file_range_t	content_range	= http_rephd.content_range(&totfile_len);
	if( totfile_len.is_null() )		return handle_itor_error("Inner uri didnt provide a totfile_len");

	// launch the bt_oload0_swarm_t 	
	bt_oload0_swarm_t *	oload0_swarm;
	oload0_swarm	= launch_oload0_swarm(totfile_len);
	if( !oload0_swarm ){
		// autodelete - no handle_itor_error() as all the httpo_full_db are now owned by bt_oload0_swarm_t
		nipmem_delete	this;
		return false;
	}

	// put the http_sclient connection to the http_client_pool_t of the bt_http_ecnx_t
	// - NOTE: it may be reused by bt_http_ecnx_pool_t and this connection
	//   establishement of the original connection wont be wasted.
	bt_http_ecnx_pool_t *http_ecnx_pool	= oload0_swarm->get_http_ecnx_pool();
	getlen_sclient->forward_cnx_to_pool(http_ecnx_pool->get_http_client_pool());

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
 * @return false for the convenience to simulate a dontkeep for the bt_oload0_stat_itor_t
 */
bool	bt_oload0_stat_itor_t::handle_itor_error(const std::string &reason)	throw()
{
	bt_oload0_t *		bt_oload0	= oload0_stat->bt_oload0;
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





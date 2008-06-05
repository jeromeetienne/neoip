/*! \file
    \brief Definition of the \ref bt_oload0_mlink_itor_t


\Par Brief Description
- it handle the itor part for the bt_oload0_tort_t
  -# it download the mlinkent at the inner_uri
  -# it create a bt_mfile_t base on it
  -# it create the bt_oload0_swarm_t for it

\par Limitation
- it works only for single file mlinkent. all multifile mlinkent will reply an error
  in all the attached bt_httpo_full_t.

\par Possible Improvement - access multi file .mlinkent
- to make a way to access multi file .mlinkent
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
#include "neoip_bt_oload0_mlink_itor.hpp"
#include "neoip_bt_oload0_mlink.hpp"
#include "neoip_bt_oload0_swarm.hpp"
#include "neoip_bt_oload0_herr.hpp"
#include "neoip_bt_oload0.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_mlink_file_helper.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_nested_uri.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_xml_parse_doc.hpp"
#include "neoip_xml_parse.hpp"
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
bt_oload0_mlink_itor_t::bt_oload0_mlink_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	oload0_mlink	= NULL;
	mlink_sclient	= NULL;
	subfile_idx	= std::numeric_limits<size_t>::max();
}

/** \brief Destructor
 */
bt_oload0_mlink_itor_t::~bt_oload0_mlink_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_oload0_mlink_t
	if( oload0_mlink )	oload0_mlink->stat_itor_unlink(this);
	// delete the httpo_full_db if needed
	while( !httpo_full_db.empty() ){
		nipmem_delete	httpo_full_db.front();
		httpo_full_db.pop_front();
	}
	// delete the mlink_sclient if needed
	nipmem_zdelete	mlink_sclient;
	// delete the getlen_sclient_db if needed
	while( !getlen_sclient_db.empty() ){
		nipmem_delete	getlen_sclient_db.front();
		getlen_sclient_db.pop_front();
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t bt_oload0_mlink_itor_t::start(bt_oload0_mlink_t *oload0_mlink, const http_uri_t &nested_uri
						, bt_httpo_full_t *httpo_full)	throw()
{
	const bt_oload0_mlink_profile_t &profile		= oload0_mlink->get_profile();
	http_uri_t			inner_uri	= http_nested_uri_t::parse_inner(nested_uri);	
	http_err_t			http_err;
	
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->oload0_mlink	= oload0_mlink;
	this->nested_uri	= nested_uri;
	// link this object to the bt_oload0_mlink_t
	oload0_mlink->stat_itor_dolink(this);

	// add this bt_httpo_full_t
	add_httpo_full(httpo_full);

	// launch the http_client_t to get the mlink_file_t from the inner_uri
	mlink_sclient	= nipmem_new http_sclient_t();
	http_err	= mlink_sclient->set_profile(profile.http_sclient()).start(inner_uri, this, NULL);
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
void	bt_oload0_mlink_itor_t::add_httpo_full(bt_httpo_full_t *httpo_full)	throw()
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
//                   Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build the bt_mfile_t for this connection based on the mlink_file_t and subfile_idx
 */
bt_mfile_t	bt_oload0_mlink_itor_t::build_bt_mfile()	const throw()
{
#if 0	// TODO to remove - kept around the time to test the new mlink_file_helper_t::to_bt_mfile()
	// sanity check - the mlink_file_t MUST be init
	DBG_ASSERT( !mlink_file.is_null() );
	// sanity check - the subfile_idx MUST be init
	DBG_ASSERT( subfile_idx < mlink_file.subfile_arr().size() );
	// set some variable
	const bt_oload0_mlink_profile_t &profile		= oload0_mlink->get_profile();
	const mlink_subfile_t &		mlink_subfile	= mlink_file.subfile_arr()[subfile_idx];
	const mlink_url_arr_t &		mlink_url_arr	= mlink_subfile.url_arr();
	http_uri_t			inner_uri	= http_nested_uri_t::parse_inner(nested_uri);	
	bt_mfile_t			bt_mfile;
	// set the bt_mfile.name() - not required but nice to have in wikidbg :)
	bt_mfile.name		( inner_uri.path().basename() / mlink_subfile.name() );

	// set the piece_len - with a quite short piece_len to improve the sharing
	bt_mfile.piecelen	( profile.mfile_piecelen() );

	// build the infohash
	bt_mfile.infohash	( build_infohash() );

	// sanity check - the file_size MUST NOT be null
	DBG_ASSERT( !mlink_subfile.file_size().is_null() );

	// create the mfile_subfile
	bt_mfile_subfile_t	mfile_subfile;	
	mfile_subfile		= bt_mfile_subfile_t(mlink_subfile.name(), mlink_subfile.file_size());
	// add all the mlink_url_t of type http
	for(size_t url_idx = 0; url_idx != mlink_url_arr.size(); url_idx++){
		const	mlink_url_t &	mlink_url	= mlink_url_arr[url_idx];
		// if this mlink_url_t is not of type "http", goto the next
		if( mlink_url.type() != "http" )	continue;
		// add this url to the uri_arr of this mfile_subfile
		mfile_subfile.uri_arr()	+= mlink_url.url_content();
	}
	// sanity check - the mfile_subfile.uri_arr() MUST NOT be empty
	// - if no mlink_url is of type "http", this mlink_file_t MUST be discarded before
	DBG_ASSERT( !mfile_subfile.uri_arr().empty() );

	// add this bt_mfile_subfile_t to the bt_mfile.subfile_arr
	bt_mfile.subfile_arr()	+= mfile_subfile;
	
	// set the bt_mfile.do_piecehash to false as piecehash_arr is empty
	bt_mfile.do_piecehash(false);
	// complete the bt_mfile_t initialization
	bt_mfile.complete_init();
#else
	const bt_oload0_mlink_profile_t &profile		= oload0_mlink->get_profile();
	http_uri_t	inner_uri	= http_nested_uri_t::parse_inner(nested_uri);	
	std::string	mfile_name	= inner_uri.path().basename().to_string();
	bt_mfile_t	bt_mfile;
	// build the bt_mfile_t from the mlink_file_t
	bt_mfile	= mlink_file_helper_t::to_bt_mfile(mlink_file, mfile_name
				, profile.mfile_piecelen(), build_infohash(), subfile_idx);
	// complete the bt_mfile_t initialization
	bt_mfile.complete_init();
#endif	

	// return the just built bt_mfile
	return bt_mfile;
}

/** \brief Build the infohash for this bt_mfile_t
 * 
 * - derive it from the inner_uri / mlink_subfile.name()
 */
bt_id_t		bt_oload0_mlink_itor_t::build_infohash()				const throw()
{
	// sanity check - the mlink_filt_t MUST be init
	DBG_ASSERT( !mlink_file.is_null() );
	// sanity check - the subfile_idx MUST be init
	DBG_ASSERT( subfile_idx < mlink_file.subfile_arr().size() );
	// set some alias
	const bt_oload0_mlink_profile_t &profile		= oload0_mlink->get_profile();
	const mlink_subfile_t &		mlink_subfile	= mlink_file.subfile_arr()[subfile_idx];
	// derive a bt_id_t from the inner_uri / mlink_subfile.name()
	http_uri_t	inner_uri	= http_nested_uri_t::parse_inner(nested_uri);
	return bt_id_t( profile.infohash_prefix_str() + inner_uri.to_string() + "/" + mlink_subfile.name().to_string());
}

/** \brief Launch the bt_oload0_swarm_t for this bt_oload0_mlink_itor_t
 * 
 * - WARNING: this delete the bt_oload0_mlink_itor_t
 * 
 * @return always false, for convenience with the tokeep scheme
 */
bool	bt_oload0_mlink_itor_t::launch_oload0_swarm()				throw()
{
	bt_oload0_t *		bt_oload0	= oload0_mlink->bt_oload0;
	bt_oload0_swarm_t *	oload0_swarm;
	bt_err_t		bt_err;
	// build the bt_mfile_t for the bt_oload0_swarm_t
	bt_mfile_t	bt_mfile;
	bt_mfile	= build_bt_mfile();
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
		// - no handle_itor_error() as all the httpo_full_db are now owned by bt_oload0_swarm_t
		nipmem_zdelete	oload0_swarm;
	}
	// autodelete
	nipmem_delete	this;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	bt_oload0_mlink_itor_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &sclient_res)	throw() 
{
	http_sclient_t * http_sclient	= &cb_sclient;
	// if this http_sclient_t is the mlink_sclient, forward the callback
	if(http_sclient==mlink_sclient)	return mlink_sclient_cb(cb_userptr, cb_sclient, sclient_res);
	// else it is considered a getlen_sclient
	return getlen_sclient_cb(cb_userptr, cb_sclient, sclient_res);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t for mlink_file_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by mlink_sclient to provide event
 */
bool	bt_oload0_mlink_itor_t::mlink_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &sclient_res)	throw() 
{
	// log to debug
	KLOG_WARN("enter http_sclient_res="		<< sclient_res);
	KLOG_WARN("sclient_res.full_get_ok()="	<< sclient_res.full_get_ok());
	
	// if there is an error in the http_sclient_t, it is an error
	if( sclient_res.cnx_err().failed() )	return handle_itor_error("Inner HTTP error");
	if( !sclient_res.full_get_ok() )	return handle_itor_error("Inner HTTP error");
	
	// set the mlink_file_t from the reply
	// setup the xml_parse_t
	xml_parse_doc_t	xml_parse_doc;
	xml_parse_doc.set_document(sclient_res.reply_body().to_datum(datum_t::NOCOPY));
	// if the xml parsing failed, return an error
	if( xml_parse_doc.is_null() )	return handle_itor_error("mlink file is not valid xml");

KLOG_WARN("blabla");
	// parse the document into a mlink_file_t
	xml_parse_t	xml_parse(&xml_parse_doc);
	try {
		xml_parse	>> mlink_file;
	} catch(xml_except_t &e){
		return handle_itor_error("unable to parse the mlink_file_t from the reply");
	}
KLOG_WARN("blabla");
	
	// if it is not a single-file mlink_file_t, it is an error
	// - TODO it may be changed later, if the inner_uri specify a given subfile
	if( mlink_file.subfile_arr().size() != 1 )
		return handle_itor_error("The metalink contains multiple files. it is not supported");	
	
KLOG_WARN("blabla");
	// set the subfile_idx
	// - NOTE: currently always 1, will be changed later
	subfile_idx	= 0;

	// if there is no file_size for thie mlink_subfile, get one	
	const mlink_subfile_t &	mlink_subfile	= mlink_file.subfile_arr()[subfile_idx];
	
	// if there are no "http" mlink_url_t for this mlink_subfile, discard it
	const mlink_url_arr_t &		mlink_url_arr	= mlink_subfile.url_arr();
	size_t				mlink_url_idx	= 0;
	// add all the mlink_url_t of type http
	for(mlink_url_idx = 0; mlink_url_idx < mlink_url_arr.size(); mlink_url_idx++){
		const	mlink_url_t &	mlink_url	= mlink_url_arr[mlink_url_idx];
		// if this mlink_url_t is of type "http", leave the loop
		if( mlink_url.type() == "http" )	break;
	}
KLOG_WARN("blabla");
	if( mlink_url_idx == mlink_url_arr.size() )
		return handle_itor_error("The subfile of this metalink contains no http url. it is not supported");	
	
KLOG_WARN("blabla");
	// if the mlink_subfile file_size_t is unknown, try to find it out via getlen_sclient stuff
	if( mlink_subfile.file_size().is_null() ){
KLOG_WARN("blabla");
		// delete the mlink_sclient
		nipmem_zdelete	mlink_sclient;
		// launch the getlen_sclient
		bool	tokeep	= launch_getlen_sclient();
		if( !tokeep )	return false;
		// return false at the mlink_sclient has been deleted
		return false;
	}
KLOG_WARN("blabla");

	// launch the bt_oload0_swarm_t for this bt_oload0_mlink_itor_t
	return launch_oload0_swarm();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    handle all the getlen process
// - this is used to discover the file length of the subfile_idx if not provided
//   in the mlink_file_t
// - it launch a http_sclient_t for EACH mlink_url_t of type http
//   - it is very inefficient :)
//   - especially because i dont put them back in the http_sclient_pool_t afterward
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch a http_sclient_t for each http mlink_url_t of this subfile_idx
 */
bool	bt_oload0_mlink_itor_t::launch_getlen_sclient()				throw()
{
	const mlink_subfile_t &	mlink_subfile	= mlink_file.subfile_arr()[subfile_idx];
	const mlink_url_arr_t &	mlink_url_arr	= mlink_subfile.url_arr();
	http_uri_t 		http_uri;
	http_err_t		http_err;
	// add all the mlink_url_t of type http
	for(size_t i = 0; i < mlink_url_arr.size(); i++){
		const mlink_url_t &	mlink_url	= mlink_url_arr[i];
		http_sclient_t *	http_sclient;
		// if this mlink_url_t IS NOT of type "http", goto the next
		if( mlink_url.type() != "http" )	continue;
		// if this http_uri_t cant be parsed, goto the next
		http_uri	= mlink_url.url_content();
		if( http_uri.is_null() )		continue;
		// launch the http_client_t to get the length of the data
		http_sclient	= nipmem_new http_sclient_t();
		http_err	= http_sclient->start(http_reqhd_t().uri(http_uri).range(file_range_t(1,1))
											, this, NULL);
		if( http_err.failed() )	nipmem_delete	http_sclient;
		else			getlen_sclient_db.push_back(http_sclient);
	}
	KLOG_WARN("blabla");
	// if there is no http_sclient_t in the getlen_sclient_db, autodelete
	if( getlen_sclient_db.empty() ){
		KLOG_WARN("blabla");
		nipmem_delete	this;
		return false;
	}
	// return true
	return true;
}


/** \brief callback notified by getlen_sclient to provide event
 */
bool	bt_oload0_mlink_itor_t::getlen_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &sclient_res)	throw() 
{
	http_sclient_t *	getlen_sclient	= &cb_sclient;
	mlink_subfile_t &	mlink_subfile	= mlink_file.subfile_arr()[subfile_idx];
	const http_rephd_t &	http_rephd	= sclient_res.http_rephd();	
	file_size_t		instance_len;
	// log to debug
	KLOG_WARN("enter http_sclient_res="	<< sclient_res);
	KLOG_WARN("sclient_res.part_get_ok()="	<< sclient_res.part_get_ok());
	
	// handle the error of this http_sclient_t
	if( sclient_res.cnx_err().failed() )	goto error;
	if( !sclient_res.part_get_ok() )	goto error;
	
	// get the instance_len from the http_rephd
	http_rephd.content_range(&instance_len);
	if( instance_len.is_null() )		goto error;

	KLOG_WARN("intance_len=" << instance_len );

	// set the file_size_t of the mlink_subfile with the instance len
	mlink_subfile.file_size	( instance_len );

	// launch the bt_oload0_swarm_t for this bt_oload0_mlink_itor_t
	return launch_oload0_swarm();
error:
	// remove this http_client_t from the getlen_sclient_db
	getlen_sclient_db.remove(getlen_sclient);
	// delete the object itself
	nipmem_zdelete	getlen_sclient;
	// if the getlen_sclient_db is now empty, notify an error
	if( getlen_sclient_db.empty() )	return handle_itor_error("Inner HTTP error");
	// return dontkeep
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
 * @return false for the convenience to simulate a dontkeep for the bt_oload0_mlink_itor_t
 */
bool	bt_oload0_mlink_itor_t::handle_itor_error(const std::string &reason)	throw()
{
	bt_oload0_t *		bt_oload0	= oload0_mlink->bt_oload0;
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





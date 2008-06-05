/*! \file
    \brief Definition of the \ref bt_lnk2mfile_t class

\par TODO
- this is not clean code
- TODO all this link_type coded as a string is dirty... put a bt_lnk2mfile_type_t instead
- TODO this doesnt handle the bt_swarm_resumedata_t either...
  - some kludge in neoip-get to handle those
  - what about redoing all that to provide a resumedata instead of a bt_mfile_t
  - in anycase bt_swarm_resumedata_t is a  superset of bt_mfile_t
  - and then to handle bt_swarm_resumedata_t
- TODO so if refactor the plan is:
  - link_addr = std::string (http_uri or local file)
  - link_type = bt_lnk2mfile_type_t (adding RESUMEDATA in it)
  - return a bt_swarm_resumedata_t instead
  - seems all good. find a good name too :) bt_lnk2resumedata_t is no good :)
- the estimation of the file length in case of bt_lnk2mfile_type_t::STATIC
  does not use the normal HEAD technique to determine the length
  - e.g. see http://www.example-code.com/csharp/httpHeadRequest.asp

\par ALGO description (most likely obsolete - to update)
1. if the type is not provided, try to guess it
   - can it fails? should i just default to http static ?
   - after this step the link_type is known
2. retrieve the metadata
   - it can be done from a local file or via http
   - if the link_addr is a local file, simply read the file
   - if the link_addr is an http_uri_t and the link_type is not static
     retrieve the whole file.
   - if the link_addr is an http_uri_t and link type is static, do a 
     http head to estimate the size of the file to download.
     - it should returns an error if the link_type is http and the http server
       do not advertize support for request-range
3. build the bt_mfile_t based on the metadata and the link_type
   - if the link_type is static, build it on the fly
   - if the link_type is torrent, build it based on it

\par About the metalink support
- i talk with him and file may be duplicated in case of various criteria
  - the location of url = to get the closest mirror
    - may be guessed by using dnsbl (cf ipcountry_t)
  - the operating system = to get the exec for the proper target OS
    - may be assumed to be the local OS
  - the language = in case of translation to get the proper data
    - may be assumed to be the local language but... with a lot less certinity than
      the OS guess.
  - the length of the files are not required. so if not provided, this requires
    a http request with HEAD
- it should be possible to specify those values and send them to bt_lnk2mfile_t
  and use them as filter
  - aka if provided, keep only the information matching this filter
  - likely a good idea to put that in a profile
- other modifications
  - change mlink_file_t to provide something like : list of available language, or
    list of avilable OS, in case this is needed to display that to the user 
  - have the metadata in the bt_lnk2mfile_t when it notify the user.
    - thus in bt_lnk2mfile_cb_t it is possible to display the information if needed
      for him to provide another request. 

*/

/* system include */
/* local include */
#include "neoip_bt_lnk2mfile.hpp"
#include "neoip_bt_lnk2mfile_type.hpp"
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_bt_mfile_helper.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_sclient.hpp"		// TODO those include should be regrouped
#include "neoip_http_sclient_res.hpp"		// in a neoip_http.hpp
#include "neoip_http_reqhd.hpp"
#include "neoip_mlink_file.hpp"
#include "neoip_mlink_file_helper.hpp"
#include "neoip_file.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
bt_lnk2mfile_t::bt_lnk2mfile_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	http_sclient	= NULL;
}
	
/** \brief Destructor
 */
bt_lnk2mfile_t::~bt_lnk2mfile_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the http_sclient_t if needed
	nipmem_zdelete	http_sclient;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_lnk2mfile_t&	bt_lnk2mfile_t::profile(const bt_lnk2mfile_profile_t &m_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( m_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= m_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_lnk2mfile_t::start(const bt_lnk2mfile_type_t &p_link_type
				, const std::string &p_link_addr
				, bt_lnk2mfile_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_ERR("enter link_type=" << p_link_type << " link_addr=" << p_link_addr);
	// copy the parameter
	this->m_link_type	= p_link_type;
	this->link_addr		= p_link_addr;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// if the link_type is null, guess it
	if( link_type().is_null() )	m_link_type = bt_lnk2mfile_type_from_addr(link_addr);
	
	// start the processing in a zerotimer_t
	// - it avoid a callback notification during the start() if an error occurs
	// - it can happen in case of link_addr is a local file
	poststart_zerotimer.append(this, NULL);
	
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
bool	bt_lnk2mfile_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// if link_addr is a http_uri_t and link_type is 'static' and static_filelen is set
	// - bypass the initial http_sclient_t and jump directly to notify_metadata()
	if( !http_uri_t(link_addr).is_null() && !m_profile.static_filelen().is_null() 
			&& link_type() == "static" ){
		// copy the m_profile.static_filelen() to 'simulate' a http_sclient_t
		static_filelen	= m_profile.static_filelen();
		// process the metadata
		return	notify_metadata();
	}

	// if link_addr is an http_uri_t, retrieve it via http_sclient_t
	// - if the link_type is "static", it is only to retrieve the length of the content
	//   otherwise it is used to 
	if( !http_uri_t(link_addr).is_null() ){
		http_reqhd_t	http_reqhd	= http_reqhd_t().uri(link_addr);
		http_err_t	http_err;
		// if the link_type is "static", get only the first byte, else get the whole file
		if( link_type() == "static" )	http_reqhd.method(http_method_t::HEAD);
		// start the http_sclient
		http_sclient	= nipmem_new http_sclient_t();
		http_err	= http_sclient->start(http_reqhd, this, NULL);
		if(http_err.failed()) return notify_callback_failed(bt_err_from_http(http_err));
		// return tokeep
		return true;
	}
	
	// sanity check - at this point link_addr MUST NOT be a http_uri_t
	DBG_ASSERT( http_uri_t(link_addr).is_null() );
	// read the metadata_datum from a file
	file_err_t	file_err;
	file_err	= file_sio_t::readall(link_addr, metadata_datum);
	if( file_err.failed() )	return notify_callback_failed(bt_err_from_file(file_err));	
	
	// process the metadata	
	return	notify_metadata();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	bt_lnk2mfile_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &cb_sclient_res)	throw() 
{
	http_sclient_res_t	sclient_res	= cb_sclient_res;
	const http_rephd_t &	http_rephd	= sclient_res.http_rephd();
	// log to debug
	KLOG_ERR("enter http_sclient_res=" << sclient_res << " link_type=" << link_type());

	// if there is an error in the http_sclient_t, it is an error
	if( sclient_res.cnx_err().failed() )
		return notify_callback_failed("failed to retrieve metadata due to " + sclient_res.cnx_err().to_string());

	// if the link_type is "static", this http_sclient was only to get the static_filelen
	if( link_type() == "static" ){
		if(!sclient_res.full_head_ok())	return notify_callback_failed("failed to retrieve metadata");
		// if the server dont accept range-request, return error
		if(!http_rephd.accept_ranges())	return notify_callback_failed("http server do not accept range-request. and those are required");
		// get the totfile_len from the http_rephd
		static_filelen	= http_rephd.content_length();
		if( static_filelen.is_null() )	return notify_callback_failed("Unable to determine the file size");
	}else{	// otherwise it was to get the metadata_datum from http
		if(!sclient_res.full_get_ok())	return notify_callback_failed("failed to retrieve metadata");
		// copy the retrieved data into metadata_datum
		metadata_datum	= sclient_res.reply_body().to_datum(datum_t::NOCOPY);
	}
	// delete the http_sclient_res_t
	nipmem_zdelete	http_sclient;

	// process the metadata
	return	notify_metadata();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			metadata processing
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Process the metadata
 */
bool	bt_lnk2mfile_t::notify_metadata()	throw()
{
	bt_err_t	bt_err;
	// process the metadata according to the link_type
	if( link_type() == "torrent" )		bt_err	= process_metadata_torrent();
	else if( link_type() == "metalink" )	bt_err	= process_metadata_metalink();
	else if( link_type() == "static" )	bt_err	= process_metadata_static();
	else					DBG_ASSERT( 0 );
	// if an error occured, notify the caller 
	if( bt_err.failed() )	return notify_callback_failed( bt_err );	
	// notify the caller of the succeess	
	return notify_callback_succeed();
}


/** \brief generate the bt_mfile_t from the metadata_datum containing a .torrent
 * 
 * - leave the bt_swarm_resumedata_t untouched
 */
bt_err_t	bt_lnk2mfile_t::process_metadata_torrent()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// convert the .torrent datum_t into a bt_mfile_t
	bt_mfile	= bt_mfile_t::from_bencode( metadata_datum );
	if( bt_mfile.is_null() )
		return bt_err_t(bt_err_t::ERROR, "unable to parse the link as a .torrent");
	// add the local_path in the bt_mfile.subfile_arr()
	bt_mfile.set_file_local_dir( m_profile.dest_local_dir() );
#if 0	// TODO to remove - done only to test performance without piecehash checking
	bt_mfile.do_piecehash(false);
#endif
	// complete the initialization fo the bt_mfile
	bt_mfile.complete_init();
	// return no error
	return bt_err_t::OK;	
}

/** \brief generate the bt_mfile_t from the metadata_datum containing a .metalink
 * 
 * - leave the bt_swarm_resumedata_t untouched
 */
bt_err_t	bt_lnk2mfile_t::process_metadata_metalink()	throw()
{
	mlink_file_t	mlink_file;
	// setup the xml_parse_t
	xml_parse_doc_t	xml_parse_doc;
	xml_parse_doc.set_document(metadata_datum);
	// if the xml parsing failed, return an error
	if( xml_parse_doc.is_null() )	return bt_err_t(bt_err_t::ERROR, "mlink file is not valid xml");

	// parse the document into a mlink_file_t
	xml_parse_t	xml_parse(&xml_parse_doc);
	try {
		xml_parse >> mlink_file;
	} catch(xml_except_t &e){
		return bt_err_t(bt_err_t::ERROR,"unable to parse the mlink_file_t from the reply");
	}

#if 1
/* TODO sort this one out
 * - the later solution is what i call now bt_lnk2mfile_t
 * - LATER: POSSIBLE SOLUTION: get a async object with cb converting this
 *   - thus the http may discover the length
 *   - even further this object may be able to build a bt_mfile from http_uri_t or local filename
 *     in static/torrent/metalink
 *   - this is used in neoip-oload and neoip-get, no need to duplicate this especially as it is quite
 *     dirty
 * - link_addr may be a local file or a url
 * - how to get the infohash ? for a mlink_file_t with only its content
 *   - aka not the url itself
 *   - find a way to get a signature of it
 *     - maybe from the whole list of url ?
 * - to use the link_addr to derived the infohash is BAD, because many people wont
 *   have the same link_addr but still have the same mlink_file_t
 *   - e.g. relative path to the local file
 *   - e.g. the url are rather ok tho
 * - how to get the mlink_addr
 *   - this is used as destination directory when the bt_mfile_t contains several name ?
 *   - using link_addr is BAD, as it may be a http_uri_t and thus is not a valid filename
 */ 
	bt_id_t		mlink_infohash	= link_addr;
	std::string	mfile_name	= link_addr;
#endif
	bt_mfile_t	bt_mfile;
	// build the bt_mfile_t from the mlink_file_t
	// - TODO this is halfbacked. the mlink_file_helper_t should be 
	//   async to get the file length
	bt_mfile	= mlink_file_helper_t::to_bt_mfile(mlink_file, mfile_name
						, m_profile.mlink_piecelen()
						, mlink_infohash);
	if( bt_mfile.is_null() )	return bt_err_t::ERROR;
	// add the local_path in the bt_mfile.subfile_arr()
	bt_mfile.set_file_local_dir( m_profile.dest_local_dir() );
	// complete the bt_mfile_t initialization
	bt_mfile.complete_init();
	// return no error
	return bt_err_t::OK;	
}

/** \brief generate the bt_mfile_t from the link_addr containing url to a static http content
 * 
 * - leave the bt_swarm_resumedata_t untouched
 */
bt_err_t	bt_lnk2mfile_t::process_metadata_static()		throw()
{
	// log to debug
	KLOG_DBG("enter static_filelen=" << static_filelen);
	// sanity check - the static_filelen MUST have been previous set
	DBG_ASSERT( !static_filelen.is_null() );
	// convert link_addr as a bt_mfile_t
	bt_mfile	= bt_mfile_helper_t::from_http_uri(link_addr, static_filelen
						, m_profile.static_infohash_prefix()
						, m_profile.static_piecelen()
						, m_profile.static_announce_uri());
	if( bt_mfile.is_null() )	return bt_err_t::ERROR;
	// add the local_path in the bt_mfile.subfile_arr()
	bt_mfile.set_file_local_dir( m_profile.dest_local_dir() );
	// complete the bt_mfile_t initialization
	bt_mfile.complete_init();
	// return no error
	return bt_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief helper function to notify a succeess
 * 
 * - it notifies a bt_err_t::OK and take the fields directory in the object
 */
bool	bt_lnk2mfile_t::notify_callback_succeed()throw()
{
	return notify_callback(bt_err_t::OK, link_type(), bt_mfile);	
}

/** \brief helper function to notify a faillure
 */
bool	bt_lnk2mfile_t::notify_callback_failed(const std::string &reason)	throw()
{
	return notify_callback_failed(bt_err_t(bt_err_t::ERROR, reason));
}

/** \brief helper function to notify a faillure
 */
bool	bt_lnk2mfile_t::notify_callback_failed(const bt_err_t &bt_err)		throw()
{
	return notify_callback(bt_err, link_type(), bt_mfile_t());
}

/** \brief notify the callback with the tcp_event
 */
bool bt_lnk2mfile_t::notify_callback(const bt_err_t &bt_err, const bt_lnk2mfile_type_t &lnk2mfile_type
					, const bt_mfile_t &bt_mfile)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_lnk2mfile_cb(userptr, *this, bt_err, lnk2mfile_type
							, bt_mfile);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END


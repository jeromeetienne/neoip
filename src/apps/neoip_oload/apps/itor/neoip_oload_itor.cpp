/*! \file
    \brief Definition of the \ref oload_itor_t
    
\par Brief Description
oload_itor_t handles the convertion from the inner_uri to a bt_mfile_t.
additionnaly it stored all the bt_httpo_full_t for this inner_uri.
- when the operation is completed, it notifies a bt_mfile_t and bt_err_t
  - if bt_err.failed(), the convertion failed and bt_mfile_t is invalid
  - if bt_err.succeed(), the convertion succeed and bt_mfile_t is valid
    - it it up to the caller to create a oload_swarm_t for this inner_uri
      and to forward all the httpo_full_t for it.

*/

/* system include */
/* local include */
#include "neoip_oload_itor.hpp"
#include "neoip_oload_apps.hpp"
#include "neoip_oload_helper.hpp"
#include "neoip_bt_lnk2mfile.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_nested_uri.hpp"
#include "neoip_http_status.hpp"
#include "neoip_base64.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
oload_itor_t::oload_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	bt_lnk2mfile	= NULL;
}

/** \brief Destructor
 */
oload_itor_t::~oload_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_oload_apps_t
	if( oload_apps )	oload_apps->itor_unlink(this);
	// delete the httpo_full_db with http_status_t::Service Unavailable - rfc2616.10.5.4
	httpo_full_db_dtor(http_status_t::SERVICE_UNAVAILABLE);
	// delete the bt_lnk2mfile_t if needed
	nipmem_zdelete	bt_lnk2mfile;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t oload_itor_t::start(oload_apps_t *oload_apps, const http_uri_t &nested_uri
				, bt_httpo_full_t *httpo_full
				, oload_itor_cb_t *callback, void *userptr)	throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_WARN("enter");
	// sanity check - the nested_uri MUST be valid
	DBG_ASSERT( http_nested_uri_t::is_valid_nested(nested_uri) );
	// copy the parameter
	this->oload_apps	= oload_apps;
	this->m_inner_uri	= oload_helper_t::parse_inner_uri(nested_uri);
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the bt_oload_apps_t
	oload_apps->itor_dolink(this);

	// add this bt_httpo_full_t
	httpo_full_push(httpo_full);
	
	// extrace the link_type from the outter_uri variables if any
	// - it is not present, set bt_lnk2mfille_type_t to null
	http_uri_t 	outter_uri	= oload_helper_t::parse_outter_uri(nested_uri);
	bt_lnk2mfile_type_t link_type	= outter_uri.var().get_first_value("link_type");
	

	// build the lnk2mfile_profile_t according to the nested_uri
	bt_lnk2mfile_profile_t	lnk2mfile_profile; 
	bt_err		= lnk2mfile_profile_update(lnk2mfile_profile, nested_uri);
	if( bt_err.failed() )	return bt_err;

	// start the bt_lnk2mfile_t
	bt_lnk2mfile	= nipmem_new bt_lnk2mfile_t();
	bt_err		= bt_lnk2mfile->profile(lnk2mfile_profile)
					.start(link_type, inner_uri().to_string(), this, NULL);
	if( bt_err.failed() )	return bt_err;
	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a bt_lnk2mfile_profile_t to use with this nested_uri
 * 
 * - in short this set it according to the nested_uri outter_var
 */
bt_err_t	oload_itor_t::lnk2mfile_profile_update(bt_lnk2mfile_profile_t &lnk2mfile_profile
					, const http_uri_t &nested_uri)		throw()
{
	http_uri_t		outter_uri	= oload_helper_t::parse_outter_uri(nested_uri);
	const strvar_db_t &	outter_var_db	= outter_uri.var();

	// handle the piecelen outter_var
	if( outter_var_db.contain_key("piecelen") ){
		const std::string	value_str	= outter_var_db.get_first_value("piecelen");
		size_t			piecelen	= string_t::to_uint32(value_str);
		// if the piecelen is 0, return an error
		if( piecelen == 0 )		return bt_err_t(bt_err_t::ERROR, "piecelen outter_var MUST NOT be 0");
		// if the piecelen is NOT 16k aligned, return an error
		if( piecelen % (16*1024) )	return bt_err_t(bt_err_t::ERROR, "piecelen outter_var MUST be a multiple of 16kbyte");
		// copy the piecelen into the profile
		// - TODO should i copy it for mlink too ? likely yes. but anyway mlink implementation
		//   is a mess :)
		lnk2mfile_profile.static_piecelen(piecelen);
	}

	// handle the static_filelen outter_var
	// - TODO this variable SHOULD NOT be in a profile... this is more like a _arg..
	//   - i dunno and currently the profile exists and not the _arg.. so i keep
	//     using it. but 'unconfortable'
	if( outter_var_db.contain_key("static_filelen") ){
		const std::string	value_str	= outter_var_db.get_first_value("static_filelen");
		file_size_t		static_filelen	= string_t::to_uint64(value_str);
		lnk2mfile_profile.static_filelen(static_filelen);
	}
	
	// handle the http_peersrc_uri outter_var - MUST be encoded in base64 safe_uri
	if( outter_var_db.contain_key("http_peersrc_uri") ){
		const std::string	value_str	= outter_var_db.get_first_value("http_peersrc_uri");
		http_uri_t		announce_uri	= base64_t::decode_safe(value_str).to_stdstring();
		// if the piecelen is 0, return an error
		if( announce_uri.is_null() )	return bt_err_t(bt_err_t::ERROR, "http_peersrc_uri outter_var is unparsable");
		// copy the announce_uri into the profile
		lnk2mfile_profile.static_announce_uri(announce_uri);
	}

#if 0
// TODO how to extrace inner_var_db ?
// - this is all inner_uri variable which start with "neoip_inneruri_var_" prefix
//   - or any big prefix like that
// - those variable are usable in inner_uri and all dupuri
// - those variables MUST be removed before sending them to actual server
//   - this mean in bt_ecnx_vapi_t and in bt_lnk2mfile_t
// - how to code that ?
	// if ecnx_rangereq_type is not "default", set profile.static_must_rangereq to false
	std::string	ecnx_rangereq_type;
	ecnx_rangereq_type	= inner_var_db.get_first_value("ecnx_rangereq_type", "default");
	if( ecnx_rangereq_type != "default" )	lnk2mfile_profile.static_must_rangereq(false);
#endif
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the nested_uri for this oload_itor_t
 * 
 * - in fact this is the requested http_uri_t from the first queued bt_httpo_full_t
 * - WARNING: if  
 */
const http_uri_t &	oload_itor_t::nested_uri()	const throw()
{
	// sanity check - m_httpo_full_db MUST NOT be empty
	DBG_ASSERT( !m_httpo_full_db.empty() ); 
	// get the first queued bt_httpo_full_t
	bt_httpo_full_t * httpo_full	= m_httpo_full_db.front();
	// get its requested http_uri_t
	const http_uri_t &nested_uri	= httpo_full->http_reqhd().uri();
	// return the nested_uri
	return nested_uri;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			httpo_full_db management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief push a new bt_httpo_full_t on this oload_itor_t
 */
void	oload_itor_t::httpo_full_push(bt_httpo_full_t *httpo_full)	throw()
{
	// add the new httpo_full to the database
	m_httpo_full_db.push_back(httpo_full);
}

/** \brief pop a bt_httpo_full_t from the oload_itor
 */
bt_httpo_full_t *	oload_itor_t::httpo_full_pop()			throw()
{
	// if the database is empty, return null
	if( m_httpo_full_db.empty() )	return NULL;
	// get the first of the database
	bt_httpo_full_t * httpo_full	= m_httpo_full_db.front();
	// remove it from the database
	m_httpo_full_db.pop_front();
	// return its value
	return httpo_full;	
}

/** \brief Delete the whole http_full_db returning a http error if needed
 */
void	oload_itor_t::httpo_full_db_dtor(const http_status_t &status_code
				, const std::string &reason
				, const http_uri_t &redir_uri)	throw()
{
	// delete the httpo_full_db if needed
	while( !m_httpo_full_db.empty() ){
		bt_httpo_full_t * httpo_full	= m_httpo_full_db.front();
		// remote it from the m_httpo_full_db 
		m_httpo_full_db.pop_front();
		// reply the http error
		oload_helper_t::reply_err_httpo_full(httpo_full, status_code, reason, redir_uri);
		// delete the object itself
		nipmem_zdelete httpo_full;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_lnk2mfile_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_lnk2mfile_t to provide event
 */
bool	oload_itor_t::neoip_bt_lnk2mfile_cb(void *cb_userptr, bt_lnk2mfile_t &cb_bt_lnk2mfile
				, const bt_err_t &bt_err
				, const bt_lnk2mfile_type_t &lnk2mfile_type
				, const bt_mfile_t &cb_bt_mfile)		throw() 
{
	bt_mfile_t	bt_mfile	= cb_bt_mfile;
	// log to debug
	KLOG_ERR("enter bt_err="<< bt_err << " link2mfile=" << lnk2mfile_type);

	// if bt_lnk2mfile_t failed and was a bt_lnk2mfile_type_t::STATIC, do a redirect
	// - see rfc2616.10.3
	// - NOTE: this is usefull in case the http server doesnt support range 
	//   request, it will be rejected.
	if( bt_err.failed() && lnk2mfile_type == bt_lnk2mfile_type_t::STATIC ){
		httpo_full_db_dtor(http_status_t::MOVED_PERMANENTLY, "", inner_uri());
	}else if( bt_err.failed() ){	
		// if bt_lnk2mfile_t failed, notify all bt_httpo_full_t of the proper reason
		httpo_full_db_dtor(http_status_t::NOT_FOUND, bt_err.reason());
	}

	// update the bt_mfile_t with ALL the 'dupuri' variable found in outter_uri
	if( bt_err.succeed() ){
		oload_helper_t::update_bt_mfile_with_dupuri(bt_mfile, nested_uri());
	}

	// simply forward the result to the caller
	// - NOTE: up to it to start a oload_swarm_t and to have
	//   the bt_httpo_full_t attached to it
	return notify_callback(bt_err, bt_mfile);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool oload_itor_t::notify_callback(const bt_err_t &bt_err, const bt_mfile_t &bt_mfile)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_oload_itor_cb(userptr, *this, bt_err, bt_mfile);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;





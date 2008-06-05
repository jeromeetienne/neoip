/*! \file
    \brief Definition for handling the convertion of bt_mfile_t from/to torrent file

\par About the subfile handling in the nested_uri
- subfile is a feature which allow to select a given file within a multi-file bt_mfile.
  - this is important because http is able to deliver only a single file and this
    feature allows to deliver file from inside a multi-file bt_mfile.
  - additionnaly this may hint the downloader about the name of the downloaded file
- generation of a subfile
  - there is a outter_uri variable called "subfile_level" which is equal to an integer
    - its value is the number of path level of the subfile e.g. my/path/file is 3
  - the subfile_path must be appended to the path section of the inner_uri 
    - aka *before* the query section (starting with a "?")
- parsing of a subfile
  - if the parser doesnt find a subfile_level outter variable, do nothing
  - else get its value as an integer
  - push a 'internal-only' outter_var called "subfile_path" which is a string equal
    to the last subfile_level of the nested_uri's path
  - remove the last subfile_level of the nested_uri's path
- this option is generally used along with the "link_type" outter_var in order to 
  allow neoip-oload to properly determine the link_type
  - the precense of the subfile_path is likely to confuse the auto-diagnostic of neoip-oload
- Generation example:
  - say outter_uri = "http://outter.site.org/"
  - say inner_uri  = "http://inner.site.org/bla.torrent?dummyvar"
  - say one want the file "my/great/file" inside bla.torrent
  - the nested_uri should be "http://outter.site.org/_*subfile_level*3/http/inner.site.org/bla.torrent/my/great/file?dummyvar"
    - because "my/great/file" has 3 level, so subfile_level = 3
- parsing example:
  - test for presence of a "subfile_level" outter variable, it exists and equal 3
  - create a 'internal-only' outter var called subfile_path equal to "my/great/file"
  - remove the last 3 level of the nested_uri path


\par About the dupuri
- it is possible to specify one or more dupuri as outter variable in
  a nested_uri for neoip-oload.
- those dupuri are used as bt_ecnx_vapi_t, so they must be http_uri_t 
- it is possible to specify several of them
- one trick is about the encoding of the outter_var value 
  - as the outter_var is encoded inside the outter_uri path, to put the plain
    value of the dupuri would lead to conflict in the charset because the dupuri
    will contains characteres like "/" or "?" etc...
  - in order to avoid those escaping issue, the value of the dupuri is encoded
    in base64 using the "safe alphabet" 
  - as in rfc3548.4 "Base 64 Encoding with URL and Filename Safe Alphabet"
    - the code is in base64_t

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_oload_helper.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_http_nested_uri.hpp"
#include "neoip_base64.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			nested uri parser
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the inner_uri from a nested_uri with subfile outter_uri variable parsing
 */
http_uri_t	oload_helper_t::parse_inner_uri(const http_uri_t &nested_uri)	throw()
{
	// parse the nested_uri into outter_uri/inner_uri 
	http_uri_t	outter_uri	= http_nested_uri_t::parse_outter(nested_uri);
	http_uri_t	inner_uri	= http_nested_uri_t::parse_inner(nested_uri);
	// parse the subfile if needed
	process_subfile(outter_uri, inner_uri);
	// return the inner_uri
	return inner_uri;	
}

/** \brief Parse the outter_uri from a nested_uri with subfile outter_uri variable parsing
 */
http_uri_t	oload_helper_t::parse_outter_uri(const http_uri_t &nested_uri)	throw()
{
	// parse the nested_uri into outter_uri/inner_uri 
	http_uri_t	outter_uri	= http_nested_uri_t::parse_outter(nested_uri);
	http_uri_t	inner_uri	= http_nested_uri_t::parse_inner(nested_uri);
	// parse the subfile if needed
	process_subfile(outter_uri, inner_uri);
	// return the outter_uri
	return outter_uri;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			subfile stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the subfile_idx pointed by this nested_uri inside this bt_mfile_t
 * 
 * - NOTE: private to oload_helper_t
 */
size_t	oload_helper_t::find_matching_subfile_idx(const bt_mfile_t &bt_mfile
						, const http_uri_t &nested_uri)	throw()
{
	// parse the nested_uri into outter_uri/inner_uri 
	http_uri_t	outter_uri	= http_nested_uri_t::parse_outter(nested_uri);
	http_uri_t	inner_uri	= http_nested_uri_t::parse_inner(nested_uri);
	// parse the subfile if needed
	process_subfile(outter_uri, inner_uri);
	
	// if outter_uri has not 'subfile' variable, just return 0
	if( outter_uri.var().contain_key("subfile_level") == false ){
		// if bt_mfile_t has a single file, return the subfile_idx 0
		if( bt_mfile.subfile_arr().size() == 1 )	return 0; 
		// if bt_mfile_t has multiple files, return an error
		return std::numeric_limits<size_t>::max();
	}
	
	// get the subfile_path from the outter_uri variable
	std::string	subfile_path	= outter_uri.var().get_first_value("subfile_path");
	DBG_ASSERT( !subfile_path.empty() );
	 
	// go thru the whole bt_mfile_t::subfile_arr to find the 
	const bt_mfile_subfile_arr_t &	subfile_arr	= bt_mfile.subfile_arr();
	for(size_t i = 0; i < subfile_arr.size(); i++){
		const file_path_t & mfile_path	= subfile_arr[i].mfile_path();
		// log to debug
		KLOG_ERR("mfile_path=" << mfile_path << " subfile_path=" << subfile_path << " subfile_idx=" << i);
		// if the mfile_path IS EQUAL to subfile_path, return its idx
		if( mfile_path == subfile_path )	return i; 
	}
	// if this point is reached, this is an error, so return an error
	return std::numeric_limits<size_t>::max();
}

/** \brief Return the subfile_idx pointed by this nested_uri inside this bt_mfile_t
 * 
 * - WARNING: it assert if none is found.
 */
size_t	oload_helper_t::subfile_idx_by_nested_uri(const bt_mfile_t &bt_mfile
						, const http_uri_t &nested_uri)	throw()
{
	// try to a subfile_idx matching the bt_mfile_t and the nested_uri
	size_t	subfile_idx	= find_matching_subfile_idx(bt_mfile, nested_uri);
	// sanity check - subfile_idx MUST NOT be std::numeric_limits<size_t>::max()
	DBG_ASSERT( subfile_idx != std::numeric_limits<size_t>::max() );
	// sanity check - subfile_idx MUST be < bt_mfile.subfile_arr().size()
	DBG_ASSERT( subfile_idx < bt_mfile.subfile_arr().size() );
	// return the subfile_idx
	return subfile_idx;
}

/** \brief Return true if it is possible to find the subfile_idx matching this nested_uri
 */
bool	oload_helper_t::has_subfile_idx_by_nested_uri(const bt_mfile_t &bt_mfile
						, const http_uri_t &nested_uri)	throw()
{
	// try to a subfile_idx matching the bt_mfile_t and the nested_uri
	size_t	subfile_idx	= find_matching_subfile_idx(bt_mfile, nested_uri);
	// if subfile_idx is std::numeric_limits<size_t>::max(), return false
	if( subfile_idx == std::numeric_limits<size_t>::max() )	return false;
	// if all previous tests passed, return true
	return true;
}

/** \brief process outter_uri and inner_uri to remove the subfile path from the inner_uri
 * 
 * - if an error occurs, outter_uri/inner_uri are set to null http_uri_t
 * 
 * - TODO this is a poor name, find a better one
 * - There is a special case/variable where is it possible to put a path at the 
 *   end of the nested_uri
 * - this aims to provide more human readable http_uri_t
 * - this aims to able to tune the basename of the nested_uri (it is usefull in 
 *   some special cases 
 * - example:
 *   nested_uri	= http://jmehost1/ *subfile*1/http/jmehost2/slota.torrent/favoritesubfile.avi"
 *   inner_uri	= http://jmehost2/slota.torrent/favdir
 *   outter_uri	= http://jmehost1/?subfile=1&subfile_path=%2ffavoritesubfile.avi
 * - the subfile=1 indicated the number of 'name' in the path to remove at the end of
 *   of the nested http_uri_t.
 * - those names are removed and placed in a outter_uri variable "subfile_path"
 */
void	oload_helper_t::process_subfile(http_uri_t &outter_uri, http_uri_t &inner_uri)throw()
{
	// if outter_uri has not 'subfile' variable, do nothing and return now
	if( outter_uri.var().contain_key("subfile_level") == false )	return;

	// compute the subfile_level
	size_t	subfile_level	= string_t::to_uint32(outter_uri.var().get_first_value("subfile_level"));

	// if the subfile_level is not valid, return a null http_uri_t 
	if( inner_uri.path().size() < subfile_level ){
		inner_uri	= http_uri_t();
		outter_uri	= http_uri_t();
		return;
	}

	/*
	 * set the outter_uri subfile_path
	 */
	// build the subfile_path
	std::ostringstream	subfile_path;
	for(size_t i = inner_uri.path().size()-subfile_level; i < inner_uri.path().size();i++){
		if( i != inner_uri.path().size()-subfile_level )	subfile_path 	<< "/";
		subfile_path 	<< inner_uri.path()[i].to_string();
	}
	// append the value to the outter_uri variable
	outter_uri.var().update("subfile_path", subfile_path.str());

	/*
	 * Remove the subfile_path from the inner_uri
	 */
	std::ostringstream 	inner_oss;
	inner_oss << "http://" << inner_uri.hostport_str() << "/";
	for(size_t i = 0; i < inner_uri.path().size() - subfile_level; i++){
		inner_oss << inner_uri.path()[i];
		inner_oss << "/";
	}
	inner_oss << inner_uri.query_str();
	// update the inner_uri
	inner_uri = http_uri_t(inner_oss.str());
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			dupuri stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Update a bt_mfile_t with ALL the dupuri found in the outter_uri variables
 * 
 * - if an error occurs, set bt_mfile to null
 */
void	oload_helper_t::update_bt_mfile_with_dupuri(bt_mfile_t &bt_mfile
						, const http_uri_t &nested_uri)	throw()
{
	std::list<http_uri_t>	dupuri_db	= extract_dupuri_db(nested_uri);
	size_t			subfile_idx	= subfile_idx_by_nested_uri(bt_mfile, nested_uri);
	bt_mfile_subfile_t &	mfile_subfile	= bt_mfile.subfile_arr()[subfile_idx];
	// if the dupuri_db is empty, return now
	if( dupuri_db.empty() )	return;
	// go thru the whole dupuri_db
	std::list<http_uri_t>::iterator	iter;
	for( iter = dupuri_db.begin(); iter != dupuri_db.end(); iter++){
		const http_uri_t &	dupuri	= *iter;
		// append this dup_uri to the mfile_subfile.uri_arr
		mfile_subfile.uri_arr().append(dupuri);	
	}
	// do/redo a bt_mfile_t::complete_init
	bt_mfile.complete_init();
}

/** \brief return a list of http_uri_t of all the dupuri found in the nested_uri
 */
std::list<http_uri_t>	oload_helper_t::extract_dupuri_db(const http_uri_t &nested_uri) throw()
{
	http_uri_t	outter_uri	= parse_outter_uri(nested_uri);
	size_t		keyidx		= strvar_db_t::INDEX_NONE;
	std::list<http_uri_t>	http_uri_db;
	while(true){
		// get the next 'dupuri' key
		keyidx	= outter_uri.var().next_key_idx("dupuri", keyidx);
		// if none is remaining, leave the loop
		if( keyidx == strvar_db_t::INDEX_NONE )	break;
		// get the value of this 'dupuri' variable
		std::string	val_str	= outter_uri.var()[keyidx].val();
		// decode it with the base64_t::safe_alphabet
		val_str		= base64_t::decode_safe(val_str).to_stdstring();
		// convert the val_str into a http_uri_t
		http_uri_t	dup_uri	= val_str;
		if( dup_uri.is_null() ){
			KLOG_ERR("found dup_uri=" << val_str << " and unable to convert it as http_uri_t");
			return std::list<http_uri_t>();
		}
		// log to debug
		KLOG_ERR("dup_uri=" << dup_uri);
		// append this dup_uri to the http_uri_db
		http_uri_db.push_back(dup_uri);
	}
	// return the http_uri_db
	return http_uri_db;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			oload_mod_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the oload_mod_vapi_t for this bt_httpo_full_t
 */
oload_mod_vapi_t *oload_helper_t::mod_vapi_from(const bt_httpo_full_t * httpo_full) throw()
{
	// get the userptr from the bt_httpo_full_t
	void *	userptr	= httpo_full->userptr();
	// sanity check - the userptr of bt_httpo_full_t MUST NEVER be null in neoip-oload
	// - aka it always contains the oload_mod_vapi_t for this bt_httpo_full_t
	DBG_ASSERT( userptr );
	// convert it to a oload_mod_vapi_t and return it
	return (oload_mod_vapi_t *)userptr;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Reply an error http_rephd_t to this httpo_full_t
 * 
 * - this is used to reply an error just before deleting the bt_httpo_full_t
 */
void	oload_helper_t::reply_err_httpo_full(bt_httpo_full_t * httpo_full
					, const http_status_t &status_code
					, const std::string &reason_phrase
					, const http_uri_t & redir_uri)	throw()
{
	http_rephd_t	http_rephd; 
	// build the http_rephd_t
	http_rephd.version(http_version_t::V1_1).status_code(status_code.get_value())
				.reason_phrase(reason_phrase);

	// if there is a redir_uri, put it in the "Location" field
	// - see rfc2616.10.3.2 redirection
	if( !redir_uri.is_null() )
		http_rephd.header_db().append("Location", redir_uri.to_string());

	// Start the bt_httpo_full_t but in reply error
	httpo_full->start_reply_error( http_rephd );
}

NEOIP_NAMESPACE_END;







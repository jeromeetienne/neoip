/*! \file
    \brief Definition for handling the convertion of mlink_file_t to bt_mfile_t

\par Possible Improvement
- mlink_file_helper_t::to_bt_mfile() fails if the one file doesnt specify file_size_t
  - TODO it may be worked around by retrieving the length with an http request
  - it would require to make this function an async operation
  - neoip_oload already does it internally and neoip_get doesnt do it 

*/

/* system include */
/* local include */
#include "neoip_mlink_file_helper.hpp"
#include "neoip_mlink_file.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         torrent file convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert a mlink_file_t to a bt_mfile_t
 * 
 * - the resulting bt_mfile_t is filled enought to call bt_mfile.complete_init() on it
 *   but it is up to the caller to call it
 * - it may return a null bt_mfile_t if an error occurs
 * - build a minimal bt_mfile_t
 *   - set the bt_mfile name/piecelen/infohash
 *   - set bt_mfile_subfile_t for mlink_subfile_t with filling the bt_mfile_subfile_t::uri_arr()
 *     with the mlink_url_t of type "http" for this mlink_subfile_t
 * - NOTE: rather kludgy due to the halfbacked nature of metalink standard
 *   - e.g. if any mlink_subfile_t has no specified size, as it is allowed by the standard
 *     it is impossible to put it in a bt_mfile_t, so an error is returned
 *     - this one is a clear mistake of the 'standard'
 *     - TODO it may be worked around by retrieving the length with an http request
 *       - it would require to make this function an async operation
 *       - neoip_oload already does it internally and neoip_get doesnt do it 
 *   - e.g. if any mlink_subfile_t has no http link, it is impossible to put it in a 
 *     bt_mfile_t, so an error is returned.
 *     - this one is a lot more an issue in my implemenation, not supporting https:// or ftp://
 *   - another issue with my implementation is the absence of torrent file support
 *     - on the other hand the standard is rather weak on this by allowing xml description of the 
 *       torrent file without giving the spec of this xml format or even an example
 */
bt_mfile_t	mlink_file_helper_t::to_bt_mfile(const mlink_file_t &mlink_file
				, const std::string &mfile_name, size_t mfile_piecelen
				, const bt_id_t &mfile_infohash, size_t only_subfile_idx)	throw()
{
	const mlink_subfile_arr_t &	mlink_subfile_arr	= mlink_file.subfile_arr();	
	bt_mfile_t			bt_mfile;
	// sanity check - the mlink_file_t MUST be init
	DBG_ASSERT( !mlink_file.is_null() );
	// sanity check - if only_subfile_idx is set, it MUST be < mlink_subfile_arr.size()
	if( only_subfile_idx != std::numeric_limits<size_t>::max() )
		DBG_ASSERT( only_subfile_idx < mlink_subfile_arr.size() );
	// set some variable
	bt_mfile.name		( mfile_name		);
	bt_mfile.piecelen	( mfile_piecelen	);
	bt_mfile.infohash	( mfile_infohash	);

	// go thru the whole subfile_arr
	for(size_t subfile_idx = 0; subfile_idx < mlink_subfile_arr.size(); subfile_idx++){
		const mlink_subfile_t &	mlink_subfile	= mlink_subfile_arr[subfile_idx];
		// if this mlink_subfile.file_size() is null, return an error
		if( mlink_subfile.file_size().is_null() )	return bt_mfile_t();
		// if only subfile_idx is set and subfile_idx is different, goto the next
		if( only_subfile_idx != std::numeric_limits<size_t>::max() && only_subfile_idx != subfile_idx )
			continue;
		// create the mfile_subfile_t for this mlink_filesubfile_t
		bt_mfile_subfile_t	mfile_subfile;
		mfile_subfile		= bt_mfile_subfile_t(mlink_subfile.name(), mlink_subfile.file_size());
		// add all the mlink_url_t of type http
		const mlink_url_arr_t &	mlink_url_arr	= mlink_subfile.url_arr();
		for(size_t url_idx = 0; url_idx != mlink_url_arr.size(); url_idx++){
			const	mlink_url_t &	mlink_url	= mlink_url_arr[url_idx];
			// if this mlink_url_t is not of type "http", goto the next
			if( mlink_url.type() != "http" )	continue;
			// add this url to the uri_arr of this mfile_subfile
			mfile_subfile.uri_arr()	+= mlink_url.url_content();
		}
		// - if no mlink_url is of type "http", return an error
		if( mfile_subfile.uri_arr().empty() )		return bt_mfile;
		// add the just built bt_mfile_subfile_t to the bt_mfile.subfile_arr
		bt_mfile.subfile_arr()	+= mfile_subfile;
	}

	// set the bt_mfile.do_piecehash to false as piecehash_arr is empty
	bt_mfile.do_piecehash(false);
	
	// NOTE: here i dont do the bt_mfile.complete_init(), in case the caller has still stuff to do
	
	// return the just built bt_mfile_t
	return bt_mfile;
}

NEOIP_NAMESPACE_END;







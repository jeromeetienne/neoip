/*! \file
    \brief Definition for handling the convertion of bt_mfile_t from/to torrent file


*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_bt_mfile_helper.hpp"
#include "neoip_bencode.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         torrent file convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a bt_mfile_t from a file_path_t to a .torrent file
 * 
 * - just a wrapper on top of from_bencode and file_sio_t
 * - it has no error report except that any error return a null bt_mfile_t
 */
bt_mfile_t	bt_mfile_helper_t::from_torrent_file(const file_path_t &file_path)	throw()
{
	datum_t		bencoded_mfile;
	// read the whole file
	file_err_t	file_err;
	file_err	= file_sio_t::readall(file_path, bencoded_mfile);
	if( file_err.failed() )	return bt_mfile_t();
	// breturn the bt_mfile_t from the bencoded_mfile
	return bt_mfile_t::from_bencode( bencoded_mfile );
}


/** \brief store a fully init bt_mfile_t  to a file_path_t to a .torrent file
 * 
 * - just a wrapper on top of from_bencode and file_sio_t
 */
bt_err_t bt_mfile_helper_t::to_torrent_file(const bt_mfile_t &bt_mfile, const file_path_t &file_path)	throw()
{
	datum_t		bencoded_mfile	= bt_mfile.to_bencode();
	// write the whole file
	file_err_t	file_err;
	file_err	= file_sio_t::writeall(file_path, bencoded_mfile);
	if( file_err.failed() )	return bt_err_from_file(file_err);
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         from_http_uri
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a bt_mfile_t from a file_path_t to a .torrent file
 * 
 * - just a wrapper on top of from_bencode and file_sio_t
 * - it has no error report except that any error return a null bt_mfile_t
 */
bt_mfile_t bt_mfile_helper_t::from_http_uri(const http_uri_t &http_uri, const file_size_t &totfile_len
			, const std::string infohash_prefix_str, size_t piece_len
			, const http_uri_t &announce_uri)	throw()
{
	bt_mfile_t	bt_mfile;
	// log to debug
	KLOG_DBG("enter http_uri=" << http_uri << " totfile_len=" << totfile_len);
	// set the bt_mfile.name() - not required but nice to have in wikidbg :)
	bt_mfile.name		( http_uri.path().basename() );
	// set the piece_len - with a quite short piece_len to improve the sharing
	bt_mfile.piecelen	( piece_len );
	// derive the infohash from the inner_uri
	bt_mfile.infohash	( infohash_prefix_str + http_uri.to_string() );

	// build the bt_mfile_subfile_t with the inner_uri
	bt_mfile_subfile_t	mfile_subfile;
	mfile_subfile		= bt_mfile_subfile_t(http_uri.path().basename(), totfile_len);
	mfile_subfile.uri_arr()	+= http_uri;
	// add this bt_mfile_subfile_t to the bt_mfile.subfile_arr
	bt_mfile.subfile_arr()	+= mfile_subfile;

	// set bt_mfile.announce_uri, if announce_uri parameter is not null
	if( !announce_uri.is_null() )	bt_mfile.announce_uri(announce_uri); 

	// set the bt_mfile.do_piecehash to false as piecehash_arr is empty
	bt_mfile.do_piecehash(false);

	// return the just built bt_mfile_t
	return bt_mfile;
}

NEOIP_NAMESPACE_END;







/*! \file
    \brief Header for handling the convertion of bt_mfile_t from/to torrent file
    
*/


#ifndef __NEOIP_BT_MFILE_HELPER_HPP__ 
#define __NEOIP_BT_MFILE_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_uri_t;

/** \brief static helpers to manipulate bt_mfile_t
 */
class bt_mfile_helper_t {
public:
	static bt_mfile_t	from_torrent_file(const file_path_t &file_path)		throw();
	static bt_err_t		to_torrent_file(const bt_mfile_t &bt_mfile
					, const file_path_t &file_path)			throw();

	static bt_mfile_t	from_http_uri(const http_uri_t &http_uri, const file_size_t &totfile_len
					, const std::string infohash_prefix_str
					, size_t piece_len
					, const http_uri_t &announce_uri)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_MFILE_HELPER_HPP__  */











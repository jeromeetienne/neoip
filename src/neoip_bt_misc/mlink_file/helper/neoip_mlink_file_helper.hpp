/*! \file
    \brief Header for handling the convertion of mlink_file_t from/to torrent file
    
*/


#ifndef __NEOIP_MLINK_FILE_HELPER_HPP__ 
#define __NEOIP_MLINK_FILE_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	mlink_file_t;
class	bt_mfile_t;
class	bt_id_t;

/** \brief static helpers to manipulate swarm_helper in neoip_bt
 */
class mlink_file_helper_t {
public:
	static bt_mfile_t	to_bt_mfile(const mlink_file_t &mlink_file, const std::string &mfile_name
					, size_t mfile_piecelen, const bt_id_t &mfile_infohash
					, size_t only_subfile_idx = std::numeric_limits<size_t>::max())
										throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_MLINK_FILE_HELPER_HPP__  */











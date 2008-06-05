/*! \file
    \brief Header of the bt_cast_helper_t class
    
*/


#ifndef __NEOIP_BT_CAST_HELPER_HPP__ 
#define __NEOIP_BT_CAST_HELPER_HPP__ 
/* system include */
#include <stdio.h>
/* local include */
#include "neoip_http_uri.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_cast_mdata_t;
class	bt_cast_id_t;
class	bt_mfile_t;
class	bt_swarm_t;

/** \brief static helpers for libneoip_cast
 */
class bt_cast_helper_t {
public:
	static bt_mfile_t	build_mfile(const bt_cast_id_t &cast_id
					, const std::string &case_name
					, const http_uri_t &http_peersrc_uri)	throw();
	static bt_mfile_t	build_mfile(const bt_cast_mdata_t &cast_mdata)	throw();
	static void		remove_piece_outside_pieceq(bt_swarm_t *bt_swarm
					, size_t pieceq_beg, size_t pieceq_end)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_HELPER_HPP__  */




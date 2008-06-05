/*! \file
    \brief Header of the bt_httpo_full_arg_t

*/


#ifndef __NEOIP_BT_HTTPO_FULL_ARG_HPP__ 
#define __NEOIP_BT_HTTPO_FULL_ARG_HPP__ 
/* system include */
/* local include */
#include "neoip_argpack.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_datum.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_file_range.hpp"
#include "neoip_bt_pselect_slide_curs_arg.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;

NEOIP_ARGPACK_DECL_CLASS_BEGIN(bt_httpo_full_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_STUB(bt_httpo_full_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(bt_httpo_full_arg_t)
NEOIP_ARGPACK_DECL_ITEM_PTR(bt_httpo_full_arg_t	, bt_swarm_t *		, bt_swarm);
NEOIP_ARGPACK_DECL_ITEM(bt_httpo_full_arg_t	, http_rephd_t		, http_rephd)
NEOIP_ARGPACK_DECL_ITEM(bt_httpo_full_arg_t	, file_range_t		, range_tosend)
NEOIP_ARGPACK_DECL_ITEM(bt_httpo_full_arg_t	, bt_pselect_slide_curs_arg_t, curs_arg)
NEOIP_ARGPACK_DECL_ITEM(bt_httpo_full_arg_t	, datum_t		, header_datum)
NEOIP_ARGPACK_DECL_ITEM(bt_httpo_full_arg_t	, bool			, piecedel_in_dtor)
NEOIP_ARGPACK_DECL_ITEM(bt_httpo_full_arg_t	, bool			, piecedel_as_delivered)
NEOIP_ARGPACK_DECL_ITEM(bt_httpo_full_arg_t	, bool			, piecedel_pre_newlyavail)
NEOIP_ARGPACK_DECL_CLASS_END(bt_httpo_full_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPO_FULL_ARG_HPP__  */







/*! \file
    \brief Definition of the bt_httpo_full_arg_t

*/

/* system include */
/* local include */
#include "neoip_bt_httpo_full_arg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(bt_httpo_full_arg_t)
NEOIP_ARGPACK_DEF_ITEM(bt_httpo_full_arg_t	, bt_swarm_t		, bt_swarm);
NEOIP_ARGPACK_DEF_ITEM(bt_httpo_full_arg_t	, http_rephd_t		, http_rephd)
NEOIP_ARGPACK_DEF_ITEM(bt_httpo_full_arg_t	, file_range_t		, range_tosend)
NEOIP_ARGPACK_DEF_ITEM(bt_httpo_full_arg_t	, bt_pselect_slide_curs_arg_t, curs_arg)
NEOIP_ARGPACK_DEF_ITEM(bt_httpo_full_arg_t	, datum_t		, header_datum)
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(bt_httpo_full_arg_t, bool		, piecedel_in_dtor	, false)
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(bt_httpo_full_arg_t, bool		, piecedel_as_delivered	, false)
NEOIP_ARGPACK_DEF_ITEM_WDEFAULT(bt_httpo_full_arg_t, bool		, piecedel_pre_newlyavail, true)
NEOIP_ARGPACK_DEF_CLASS_END(bt_httpo_full_arg_t)

NEOIP_NAMESPACE_END


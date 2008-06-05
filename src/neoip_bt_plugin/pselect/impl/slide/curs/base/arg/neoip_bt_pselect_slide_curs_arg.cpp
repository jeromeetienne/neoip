/*! \file
    \brief Definition of the bt_pselect_slide_curs_arg_t

*/

/* system include */
/* local include */
#include "neoip_bt_pselect_slide_curs_arg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(bt_pselect_slide_curs_arg_t)
NEOIP_ARGPACK_DEF_ITEM(bt_pselect_slide_curs_arg_t, size_t		, offset)
NEOIP_ARGPACK_DEF_ITEM(bt_pselect_slide_curs_arg_t, bt_pieceprec_arr_t	, pieceprec_arr)
NEOIP_ARGPACK_DEF_ITEM(bt_pselect_slide_curs_arg_t, bool		, has_circularidx)
NEOIP_ARGPACK_DEF_CLASS_END(bt_pselect_slide_curs_arg_t)

NEOIP_NAMESPACE_END


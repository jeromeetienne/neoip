/*! \file
    \brief Header of the bt_pselect_slide_curs_arg_t

*/


#ifndef __NEOIP_BT_PSELECT_SLIDE_CURS_ARG_HPP__ 
#define __NEOIP_BT_PSELECT_SLIDE_CURS_ARG_HPP__ 
/* system include */
/* local include */
#include "neoip_argpack.hpp"
#include "neoip_bt_pieceprec_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ARGPACK_DECL_CLASS_BEGIN(bt_pselect_slide_curs_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_STUB(bt_pselect_slide_curs_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(bt_pselect_slide_curs_arg_t)
NEOIP_ARGPACK_DECL_ITEM(bt_pselect_slide_curs_arg_t	, size_t		, offset)
NEOIP_ARGPACK_DECL_ITEM(bt_pselect_slide_curs_arg_t	, bt_pieceprec_arr_t	, pieceprec_arr)
NEOIP_ARGPACK_DECL_ITEM(bt_pselect_slide_curs_arg_t	, bool			, has_circularidx)
NEOIP_ARGPACK_DECL_CLASS_END(bt_pselect_slide_curs_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PSELECT_SLIDE_CURS_ARG_HPP__  */







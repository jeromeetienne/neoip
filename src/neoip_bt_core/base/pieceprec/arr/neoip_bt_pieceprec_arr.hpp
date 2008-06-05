/*! \file
    \brief Header of the \ref bt_pieceprec_arr_t
    
*/


#ifndef __NEOIP_BT_PIECEPREC_ARR_HPP__ 
#define __NEOIP_BT_PIECEPREC_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_item_arr.hpp"
#include "neoip_bt_pieceprec.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


NEOIP_ITEM_ARR_DECLARATION_START(bt_pieceprec_arr_t, bt_pieceprec_t);
public:
	bt_pieceprec_arr_t &	set_range(size_t idx_beg, size_t idx_end, const bt_pieceprec_t & prec_beg
							, const bt_pieceprec_t & prec_end)	throw();
	bt_pieceprec_arr_t &	set_range_equal(size_t idx_beg, size_t idx_end
							, const bt_pieceprec_t &pieceprec)	throw();
	bt_pieceprec_arr_t &	set_range_decrease(size_t idx_beg, size_t idx_end
							, const bt_pieceprec_t &pieceprec)	throw();
NEOIP_ITEM_ARR_DECLARATION_END(bt_pieceprec_arr_t, bt_pieceprec_t);


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PIECEPREC_ARR_HPP__  */











/*! \file
    \brief Header of the \ref bt_cast_spos_arr_t
    
*/


#ifndef __NEOIP_BT_CAST_SPOS_ARR_HPP__ 
#define __NEOIP_BT_CAST_SPOS_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_cast_spos.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_mfile_t;

NEOIP_ITEM_ARR_DECLARATION_START(bt_cast_spos_arr_t, bt_cast_spos_t);
public:
	/*************** query function	*******************************/
	bool			contain(const file_size_t &byte_offset)		throw();
	bt_cast_spos_arr_t	within_pieceq(size_t pieceq_beg, size_t pieceq_end
						, const bt_mfile_t &bt_mfile)	const throw();
	const bt_cast_spos_t &	closest_casti_date(const date_t &casti_date)	const throw();		

	
NEOIP_ITEM_ARR_DECLARATION_END(bt_cast_spos_arr_t, bt_cast_spos_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_SPOS_ARR_HPP__  */











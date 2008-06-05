/*! \file
    \brief Header of the \ref bt_iov_arr_t

\par Some definition
- a bt_iov_arr_t is considered ordered IF all elements are sorted in increasing order
  - e.g. the bt_iov_t for the begining of the totfile are at the begining of the array.
  - e.g. the bt_iov_t for the end of the totfile are at the end of the array.
  - NOTE: a unordered bt_iov_arr_t may be ordered by bt_iov_arr_t::sort(), inherited from item_arr_t
- a bt_iov_arr_t is considered reduced IF all elements are distinct and not contiguous
  - NOTE: a unreduced bt_iov_arr_t may be reduced by bt_iov_arr_t::reduce()

*/


#ifndef __NEOIP_BT_IOV_ARR_HPP__ 
#define __NEOIP_BT_IOV_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_iov.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ITEM_ARR_DECLARATION_START(bt_iov_arr_t, bt_iov_t);
public:
	/*************** action function	*******************************/
	bt_iov_arr_t &	reduce()	throw();

NEOIP_ITEM_ARR_DECLARATION_END(bt_iov_arr_t, bt_iov_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IOV_ARR_HPP__  */











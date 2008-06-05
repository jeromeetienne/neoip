/*! \file
    \brief Header of the \ref btcli_ezswarmid_arr_t

*/


#ifndef __NEOIP_BTCLI_EZSWARMID_ARR_HPP__ 
#define __NEOIP_BTCLI_EZSWARMID_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_btcli_ezswarmid.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ITEM_ARR_DECLARATION_START(btcli_ezswarmid_arr_t, btcli_ezswarmid_t);
public:
	/*************** action function	*******************************/
	static btcli_ezswarmid_arr_t from_bt_ezsession(bt_ezsession_t *bt_ezsession)	throw();

NEOIP_ITEM_ARR_DECLARATION_END(btcli_ezswarmid_arr_t, btcli_ezswarmid_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTCLI_EZSWARMID_ARR_HPP__  */











/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_BT_IO_MODE_HPP__ 
#define __NEOIP_BT_IO_MODE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_io_mode_t		, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_io_mode_t		, SFILE)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_io_mode_t		, PFILE)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_io_mode_t		, CACHE)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_io_mode_t)
NEOIP_STRTYPE_DECLARATION_END(bt_io_mode_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_MODE_HPP__  */




/*! \file
    \brief Header of the \ref bt_lnk2mfile_type.hpp
*/


#ifndef __NEOIP_BT_LNK2MFILE_TYPE_HPP__ 
#define __NEOIP_BT_LNK2MFILE_TYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_lnk2mfile_type_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_lnk2mfile_type_t	, TORRENT)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_lnk2mfile_type_t	, STATIC)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_lnk2mfile_type_t	, METALINK)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_lnk2mfile_type_t)
NEOIP_STRTYPE_DECLARATION_END(bt_lnk2mfile_type_t	, uint8_t)


bt_lnk2mfile_type_t	bt_lnk2mfile_type_from_addr(const std::string &link_addr)	throw();


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_LNK2MFILE_TYPE_HPP__  */




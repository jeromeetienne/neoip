/*! \file
    \brief Implementation of the \ref bt_lnk2mfile_type_t
*/

/* system include */
/* local include */
#include "neoip_bt_lnk2mfile_type.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_lnk2mfile_type_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_lnk2mfile_type_t	, TORRENT	, "TORRENT")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_lnk2mfile_type_t	, STATIC	, "STATIC")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_lnk2mfile_type_t	, METALINK	, "METALINK")
NEOIP_STRTYPE_DEFINITION_END(bt_lnk2mfile_type_t)

/** \brief assigned this->link_type according to the link_addr 'file extension'
 * 
 * - if the extension is '.torrent', it is a bt_lnk2mfile_type_t::TORRENT
 * - if the extension is '.metalink', it is a bt_lnk2mfile_type_t::METALINK
 * - in all other cases, it is assumed to be a bt_lnk2mfile_type_t::STATIC
 */
bt_lnk2mfile_type_t	bt_lnk2mfile_type_from_addr(const std::string &link_addr)	throw()
{
	// if there is no extension, return bt_lnk2mfile_type_t::STATIC
	if(link_addr.find(".") == std::string::npos) 	return bt_lnk2mfile_type_t::STATIC;
	// extract the extension
	size_t		dot_pos	= link_addr.find_last_of(".");
	std::string	ext_str	= string_t::to_lower(link_addr.substr(dot_pos));
	// set the link_type according to the ext_str
	if(ext_str == ".torrent")	return bt_lnk2mfile_type_t::TORRENT;
	else if(ext_str == ".metalink")	return bt_lnk2mfile_type_t::METALINK;
	// return the default link_type
	return bt_lnk2mfile_type_t::STATIC;
}


NEOIP_NAMESPACE_END





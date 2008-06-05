/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_upnp_sockfam_HPP__ 
#define __NEOIP_upnp_sockfam_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(upnp_sockfam_t		, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(upnp_sockfam_t		, UDP)
NEOIP_STRTYPE_DECLARATION_ITEM(upnp_sockfam_t		, TCP)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(upnp_sockfam_t)
NEOIP_STRTYPE_DECLARATION_END(upnp_sockfam_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_upnp_sockfam_HPP__  */




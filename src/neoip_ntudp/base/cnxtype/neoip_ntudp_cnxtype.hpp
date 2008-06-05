/*! \file
    \brief Header of the \ref ntudp_cnxtype_t

*/


#ifndef __NEOIP_NTUDP_CNXTYPE_HPP__ 
#define __NEOIP_NTUDP_CNXTYPE_HPP__ 

/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(ntudp_cnxtype_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_cnxtype_t	, DIRECT)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_cnxtype_t	, ESTARELAY)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_cnxtype_t	, REVERSE)
NEOIP_STRTYPE_DECLARATION_ITEM(ntudp_cnxtype_t	, IMPOSSIBLE)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(ntudp_cnxtype_t)
NEOIP_STRTYPE_DECLARATION_END(ntudp_cnxtype_t	, uint8_t)


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_NTUDP_CNXTYPE_HPP__  */




/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_BT_JAMRC4_TYPE_HPP__ 
#define __NEOIP_BT_JAMRC4_TYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_jamrc4_type_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_jamrc4_type_t		, NOJAM)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_jamrc4_type_t		, DOJAM)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_jamrc4_type_t)
	bool is_nojam()		const throw() { return get_value() == NOJAM;	}
	bool is_dojam()		const throw() { return get_value() == DOJAM;	}
NEOIP_STRTYPE_DECLARATION_END(bt_jamrc4_type_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMRC4_TYPE_HPP__  */




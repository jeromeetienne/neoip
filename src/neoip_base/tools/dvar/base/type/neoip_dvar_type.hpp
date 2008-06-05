/*! \file
    \brief Header of the \ref dvar_type_t

*/


#ifndef __NEOIP_DVAR_TYPE_HPP__ 
#define __NEOIP_DVAR_TYPE_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(dvar_type_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(dvar_type_t	, INTEGER)
NEOIP_STRTYPE_DECLARATION_ITEM(dvar_type_t	, DOUBLE)
NEOIP_STRTYPE_DECLARATION_ITEM(dvar_type_t	, STRING)
NEOIP_STRTYPE_DECLARATION_ITEM(dvar_type_t	, ARRAY)
NEOIP_STRTYPE_DECLARATION_ITEM(dvar_type_t	, MAP)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(dvar_type_t)
	bool is_integer()	const throw() { return get_value() == INTEGER;	}
	bool is_dbl()		const throw() { return get_value() == DOUBLE;	}
	bool is_str()		const throw() { return get_value() == STRING;	}
	bool is_arr()		const throw() { return get_value() == ARRAY;	}
	bool is_map()		const throw() { return get_value() == MAP;	}
	bool is_atomic()	const throw() { return is_integer() || is_dbl() || is_str();	}
NEOIP_STRTYPE_DECLARATION_END(dvar_type_t	, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_TYPE_HPP__  */






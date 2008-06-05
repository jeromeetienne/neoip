/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_APPS_TYPE_HPP__ 
#define __NEOIP_APPS_TYPE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(apps_type_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(apps_type_t	, SYS_BOOT)
NEOIP_STRTYPE_DECLARATION_ITEM(apps_type_t	, USR_BOOT)
NEOIP_STRTYPE_DECLARATION_ITEM(apps_type_t	, USR_TEMP)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(apps_type_t)
	bool is_boot()	const throw()	{ return get_value() == SYS_BOOT || get_value() == USR_BOOT;	}
	bool is_temp()	const throw()	{ return get_value() == USR_TEMP;	}
	bool is_sys()	const throw()	{ return get_value() == SYS_BOOT;	}
	bool is_usr()	const throw()	{ return get_value() == USR_BOOT || get_value() == USR_TEMP;	}
	// some more readable shortcut
	bool is_system()const throw()	{ return is_sys();	}
	bool is_user()	const throw()	{ return is_usr();	}
NEOIP_STRTYPE_DECLARATION_END(apps_type_t	, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_APPS_TYPE_HPP__  */




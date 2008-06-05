/*! \file
    \brief Header of the \ref bt_io_cache_state_t
*/


#ifndef __NEOIP_BT_IO_CACHE_STATE_HPP__ 
#define __NEOIP_BT_IO_CACHE_STATE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(bt_io_cache_state_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_io_cache_state_t	, CLEAN)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_io_cache_state_t	, DIRTY)
NEOIP_STRTYPE_DECLARATION_ITEM(bt_io_cache_state_t	, CLEANING)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(bt_io_cache_state_t)

	bool	is_clean()	const throw()	{ return get_value() == CLEAN;		}
	bool	is_dirty()	const throw()	{ return get_value() == DIRTY;		}
	bool	is_cleaning()	const throw()	{ return get_value() == CLEANING;	}
	bool	is_not_clean()	const throw()	{ return !is_clean();			}
	bool	is_not_dirty()	const throw()	{ return !is_dirty();			}
	bool	is_not_cleaning()const throw()	{ return !is_cleaning();		}

NEOIP_STRTYPE_DECLARATION_END(bt_io_cache_state_t		, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_CACHE_STATE_HPP__  */




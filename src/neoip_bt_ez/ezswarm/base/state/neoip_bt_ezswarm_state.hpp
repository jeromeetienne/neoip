/*! \file
    \brief Header of the \ref bt_cmdtype.hpp
*/


#ifndef __NEOIP_BT_EZSWARM_STATE_HPP__ 
#define __NEOIP_BT_EZSWARM_STATE_HPP__ 
/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DECLARATION_START(bt_ezswarm_state_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_ezswarm_state_t	, ALLOC)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_ezswarm_state_t	, CHECK)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_ezswarm_state_t	, SHARE)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_ezswarm_state_t	, STOPPING)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_ezswarm_state_t	, STOPPED)
NEOIP_ERRTYPE_DECLARATION_ITEM(bt_ezswarm_state_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(bt_ezswarm_state_t)
	/** \brief Return true if the bt_ezswarm_state_t is endless, false otherwise
	 */
	bool is_endless()	const throw() {
		switch( get_value() ){
		case STOPPED:	case SHARE:	case ERROR:
			return true;
		case ALLOC:	case CHECK:	case STOPPING:
			return false;
		default:	DBG_ASSERT( 0 );
		return	false;	// just to avoid a compiler warning
		}
	}
	bool	is_alloc()	const throw()	{ return get_value() == ALLOC;		}
	bool	is_check()	const throw()	{ return get_value() == CHECK;		}
	bool	is_share()	const throw()	{ return get_value() == SHARE;		}
	bool	is_stopping()	const throw()	{ return get_value() == STOPPING;	}
	bool	is_stopped()	const throw()	{ return get_value() == STOPPED;	}
	bool	is_error()	const throw()	{ return get_value() == ERROR;		}
NEOIP_ERRTYPE_DECLARATION_END(bt_ezswarm_state_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_STATE_HPP__  */




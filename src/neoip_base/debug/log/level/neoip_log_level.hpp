/*! \file
    \brief Header of the \ref log_level_t

*/


#ifndef __NEOIP_LOG_LEVEL_HPP__ 
#define __NEOIP_LOG_LEVEL_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;
		
NEOIP_STRTYPE_DECLARATION_START(log_level_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(log_level_t	, EMERG)
NEOIP_STRTYPE_DECLARATION_ITEM(log_level_t	, ALERT)
NEOIP_STRTYPE_DECLARATION_ITEM(log_level_t	, CRIT)
NEOIP_STRTYPE_DECLARATION_ITEM(log_level_t	, ERR)
NEOIP_STRTYPE_DECLARATION_ITEM(log_level_t	, WARNING)
NEOIP_STRTYPE_DECLARATION_ITEM(log_level_t	, NOTICE)
NEOIP_STRTYPE_DECLARATION_ITEM(log_level_t	, INFO)
NEOIP_STRTYPE_DECLARATION_ITEM(log_level_t	, DEBUG)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(log_level_t)

	int	to_syslog()	const throw();

NEOIP_STRTYPE_DECLARATION_END(log_level_t	, uint8_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LOG_LEVEL_HPP__  */






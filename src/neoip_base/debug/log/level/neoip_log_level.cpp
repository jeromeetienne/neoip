/*! \file
    \brief Header of the \ref log_level_t

*/

/* system include */
#ifndef _WIN32
#	include <syslog.h>
#endif
/* local include */
#include "neoip_log_level.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(log_level_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(log_level_t	, EMERG		, "EMERG")
NEOIP_STRTYPE_DEFINITION_ITEM(log_level_t	, ALERT		, "ALERT")
NEOIP_STRTYPE_DEFINITION_ITEM(log_level_t	, CRIT		, "CRIT")
NEOIP_STRTYPE_DEFINITION_ITEM(log_level_t	, ERR		, "ERR")
NEOIP_STRTYPE_DEFINITION_ITEM(log_level_t	, WARNING	, "WARNING")
NEOIP_STRTYPE_DEFINITION_ITEM(log_level_t	, NOTICE	, "NOTICE")
NEOIP_STRTYPE_DEFINITION_ITEM(log_level_t	, INFO		, "INFO")
NEOIP_STRTYPE_DEFINITION_ITEM(log_level_t	, DEBUG		, "DEBUG")
NEOIP_STRTYPE_DEFINITION_END(log_level_t)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			syslog level convertion
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the syslog level matching the log_level_t one
 */
int	strtype_log_level_t::to_syslog()	const throw()
{
#ifndef _WIN32
	// convert the syslog level depending on the value of the log_level_t
	switch( get_value() ){
	case log_level_t::EMERG:	return LOG_EMERG;
	case log_level_t::ALERT:	return LOG_ALERT;
	case log_level_t::CRIT:		return LOG_CRIT;
	case log_level_t::ERR:		return LOG_ERR;
	case log_level_t::WARNING:	return LOG_WARNING;
	case log_level_t::NOTICE:	return LOG_NOTICE;
	case log_level_t::INFO:		return LOG_INFO;
	case log_level_t::DEBUG:	return LOG_DEBUG;
	default:	DBG_ASSERT( 0 );
	}
#else
	// NOTE: this should be ok, as this function is not called under _WIN32
	EXP_ASSERT( 0 );
#endif
	// NOTE: this point MUST NEVER be reached
	return -1;
}
NEOIP_NAMESPACE_END


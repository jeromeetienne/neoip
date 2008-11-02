/*! \file
    \brief Implementation of the \ref flv_soundformat_t
*/

/* system include */
/* local include */
#include "neoip_flv_soundformat.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(flv_soundformat_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundformat_t	, UNCOMPRESSED		, "UNCOMPRESSED")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundformat_t	, ADPCM			, "ADPCM")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundformat_t	, MP3			, "MP3")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundformat_t	, NELLYMOSER_8KHZ_MONO	, "NELLYMOSER_8KHZ_MONO")
NEOIP_STRTYPE_DEFINITION_ITEM(flv_soundformat_t	, NELLYMOSER		, "NELLYMOSER")
NEOIP_STRTYPE_DEFINITION_END(flv_soundformat_t)

NEOIP_NAMESPACE_END





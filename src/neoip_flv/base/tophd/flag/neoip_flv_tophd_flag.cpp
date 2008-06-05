/*! \file
    \brief Definition of \ref flv_tophd_flag_t

*/

/* system include */
/* local include */
#include "neoip_flv_tophd_flag.hpp"

NEOIP_NAMESPACE_BEGIN;


// al the definitions - typically in a .cpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)							\
		COMMAND(flv_tophd_flag_t, uint8_t, video	, VIDEO	,  0)	\
		COMMAND(flv_tophd_flag_t, uint8_t, audio	, AUDIO	,  2)

NEOIP_BITFLAG_DEFINITION	(flv_tophd_flag_t, uint8_t, NEOIP_BITFLAG_COMMAND_LIST)
#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END


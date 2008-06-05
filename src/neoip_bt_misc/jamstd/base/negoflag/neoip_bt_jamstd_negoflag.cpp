/*! \file
    \brief Definition of \ref bt_jamstd_negoflag_t

*/

/* system include */
/* local include */
#include "neoip_bt_jamstd_negoflag.hpp"

NEOIP_NAMESPACE_BEGIN;


// al the definitions - typically in a .cpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)						\
		COMMAND(bt_jamstd_negoflag_t, uint32_t, plaintext	, PLAINTEXT	, 0)	\
		COMMAND(bt_jamstd_negoflag_t, uint32_t, arcfour	, ARCFOUR	, 1)

NEOIP_BITFLAG_DEFINITION	(bt_jamstd_negoflag_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)
#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END


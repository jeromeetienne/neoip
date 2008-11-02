/*! \file
    \brief Header of the \ref bt_handshake_t

*/


#ifndef __NEOIP_FLV_TOPHD_FLAG_HPP__ 
#define __NEOIP_FLV_TOPHD_FLAG_HPP__ 
/* system include */
#include <stdint.h>
/* local include */
#include "neoip_bitflag.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// all the declaration - typically in a .hpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)					\
		COMMAND(flv_tophd_flag_t, uint8_t, video	, VIDEO	,  0)	\
		COMMAND(flv_tophd_flag_t, uint8_t, audio	, AUDIO	,  2)

NEOIP_BITFLAG_DECLARATION_START	(flv_tophd_flag_t, uint8_t, NEOIP_BITFLAG_COMMAND_LIST)
NEOIP_BITFLAG_DECLARATION_END	(flv_tophd_flag_t, uint8_t, NEOIP_BITFLAG_COMMAND_LIST)

#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_TOPHD_FLAG_HPP__  */




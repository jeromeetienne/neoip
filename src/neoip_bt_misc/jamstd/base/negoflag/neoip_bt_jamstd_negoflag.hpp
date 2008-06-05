/*! \file
    \brief Header of the \ref bt_handshake_t

*/


#ifndef __NEOIP_BT_JAMSTD_NEGOFLAG_HPP__ 
#define __NEOIP_BT_JAMSTD_NEGOFLAG_HPP__ 
/* system include */
#include <stdint.h>
/* local include */
#include "neoip_bitflag.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// all the declaration - typically in a .hpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)							\
		COMMAND(bt_jamstd_negoflag_t, uint32_t, plaintext, PLAINTEXT	, 0)	\
		COMMAND(bt_jamstd_negoflag_t, uint32_t, arcfour	, ARCFOUR	, 1)

NEOIP_BITFLAG_DECLARATION_START	(bt_jamstd_negoflag_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)
NEOIP_BITFLAG_DECLARATION_END	(bt_jamstd_negoflag_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)

#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMSTD_NEGOFLAG_HPP__  */




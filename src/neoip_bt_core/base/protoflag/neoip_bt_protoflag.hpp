/*! \file
    \brief Header of the \ref bt_handshake_t

*/


#ifndef __NEOIP_BT_PROTOFLAG_HPP__ 
#define __NEOIP_BT_PROTOFLAG_HPP__ 
/* system include */
#include <stdint.h>
/* local include */
#include "neoip_bitflag.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// all the declaration - typically in a .hpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)							\
		COMMAND(bt_protoflag_t, uint64_t, ml_dht	, ML_DHT		,  0)	\
		COMMAND(bt_protoflag_t, uint64_t, ml_fastext	, ML_FAST_EXT		,  2)	\
		COMMAND(bt_protoflag_t, uint64_t, ml_nattraversal, ML_NATTRAVERSAL	,  3)	\
		COMMAND(bt_protoflag_t, uint64_t, ut_msgproto	, UT_MSGPROTO		, 20)	\
		COMMAND(bt_protoflag_t, uint64_t, az_msgproto	, AZ_MSGPROTO		, 63)

NEOIP_BITFLAG_DECLARATION_START	(bt_protoflag_t, uint64_t, NEOIP_BITFLAG_COMMAND_LIST)
NEOIP_BITFLAG_DECLARATION_END	(bt_protoflag_t, uint64_t, NEOIP_BITFLAG_COMMAND_LIST)

#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PROTOFLAG_HPP__  */




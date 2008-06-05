/*! \file
    \brief Header of the \ref bt_handshake_t

*/


#ifndef __NEOIP_BT_EZSWARM_OPT_HPP__ 
#define __NEOIP_BT_EZSWARM_OPT_HPP__ 
/* system include */
#include <stdint.h>
/* local include */
#include "neoip_bitflag.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// all the declaration - typically in a .hpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)							\
		COMMAND(bt_ezswarm_opt_t, uint32_t, io_sfile		, IO_SFILE	,  0)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, io_pfile		, IO_PFILE	,  1)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, ecnx_http		, ECNX_HTTP	,  2)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, peersrc_http	, PEERSRC_HTTP	,  3)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, peersrc_kad		, PEERSRC_KAD	,  4)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, peersrc_nslan	, PEERSRC_NSLAN	,  5)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, peersrc_utpex	, PEERSRC_UTPEX	,  6)

NEOIP_BITFLAG_DECLARATION_START	(bt_ezswarm_opt_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)
NEOIP_BITFLAG_DECLARATION_END	(bt_ezswarm_opt_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)

#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_OPT_HPP__  */




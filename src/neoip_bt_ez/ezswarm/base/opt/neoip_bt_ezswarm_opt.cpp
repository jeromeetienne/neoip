/*! \file
    \brief Definition of the unit test for the \ref bt_ezswarm_opt_t

*/

/* system include */
/* local include */
#include "neoip_bt_ezswarm_opt.hpp"

NEOIP_NAMESPACE_BEGIN;


// al the definitions - typically in a .cpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)							\
		COMMAND(bt_ezswarm_opt_t, uint32_t, io_sfile		, IO_SFILE	,  0)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, io_pfile		, IO_PFILE	,  1)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, ecnx_http		, ECNX_HTTP	,  2)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, peersrc_http	, PEERSRC_HTTP	,  3)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, peersrc_kad		, PEERSRC_KAD	,  4)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, peersrc_nslan	, PEERSRC_NSLAN	,  5)	\
		COMMAND(bt_ezswarm_opt_t, uint32_t, peersrc_utpex	, PEERSRC_UTPEX	,  6)

NEOIP_BITFLAG_DEFINITION	(bt_ezswarm_opt_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)
#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END


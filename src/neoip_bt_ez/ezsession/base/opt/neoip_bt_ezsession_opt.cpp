/*! \file
    \brief Definition of the unit test for the \ref bt_ezsession_opt_t

*/

/* system include */
/* local include */
#include "neoip_bt_ezsession_opt.hpp"

NEOIP_NAMESPACE_BEGIN;


// al the definitions - typically in a .cpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)							\
		COMMAND(bt_ezsession_opt_t, uint32_t, kad_peer		, KAD_PEER	,  0)	\
		COMMAND(bt_ezsession_opt_t, uint32_t, nslan_peer	, NSLAN_PEER	,  1)	\
		COMMAND(bt_ezsession_opt_t, uint32_t, xmit_rsched	, XMIT_RSCHED	,  2)	\
		COMMAND(bt_ezsession_opt_t, uint32_t, recv_rsched	, RECV_RSCHED	,  3)	\
		COMMAND(bt_ezsession_opt_t, uint32_t, use_io_cache	, USE_IO_CACHE	,  4)

NEOIP_BITFLAG_DEFINITION	(bt_ezsession_opt_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)
#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END


/*! \file
    \brief Header of the \ref bt_ezsession_opt_t

- TODO rename the field
  - kad_peer is really bad name as it is a udp_vresp/kad_listener/kad_peer
    - maybe kad_base ? or something explicitly meaningless
    - better that something which seems to mean something clear but isnt really this thing
*/


#ifndef __NEOIP_BT_EZSESSION_OPT_HPP__ 
#define __NEOIP_BT_EZSESSION_OPT_HPP__ 
/* system include */
#include <stdint.h>
/* local include */
#include "neoip_bitflag.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// all the declaration - typically in a .hpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)							\
		COMMAND(bt_ezsession_opt_t, uint32_t, kad_peer		, KAD_PEER	,  0)	\
		COMMAND(bt_ezsession_opt_t, uint32_t, nslan_peer	, NSLAN_PEER	,  1)	\
		COMMAND(bt_ezsession_opt_t, uint32_t, xmit_rsched	, XMIT_RSCHED	,  2)	\
		COMMAND(bt_ezsession_opt_t, uint32_t, recv_rsched	, RECV_RSCHED	,  3)	\
		COMMAND(bt_ezsession_opt_t, uint32_t, use_io_cache	, USE_IO_CACHE	,  4)

NEOIP_BITFLAG_DECLARATION_START	(bt_ezsession_opt_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)
NEOIP_BITFLAG_DECLARATION_END	(bt_ezsession_opt_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)

#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSESSION_OPT_HPP__  */




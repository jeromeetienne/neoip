/*! \file
    \brief Definition of \ref bt_protoflag_t

\par Note About the bit definition
- i found no list of those bits - no global registery 
  - seems ultra sloppy to me
- the only source i found was BitTorrent/Connector.py in the mainline source
- the position of those bits with large empty gap between them suggest that there
  is a lot more...

*/

/* system include */
/* local include */
#include "neoip_bt_protoflag.hpp"

NEOIP_NAMESPACE_BEGIN;


// al the definitions - typically in a .cpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)							\
		COMMAND(bt_protoflag_t, uint64_t, ml_dht	, ML_DHT		, 0)	\
		COMMAND(bt_protoflag_t, uint64_t, ml_fastext	, ML_FAST_EXT		, 2)	\
		COMMAND(bt_protoflag_t, uint64_t, ml_nattraversal, ML_NATTRAVERSAL	, 3)	\
		COMMAND(bt_protoflag_t, uint64_t, ut_msgproto	, UT_MSGPROTO		, 20)	\
		COMMAND(bt_protoflag_t, uint64_t, az_msgproto	, AZ_MSGPROTO		, 63)

NEOIP_BITFLAG_DEFINITION	(bt_protoflag_t, uint64_t, NEOIP_BITFLAG_COMMAND_LIST)
#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END


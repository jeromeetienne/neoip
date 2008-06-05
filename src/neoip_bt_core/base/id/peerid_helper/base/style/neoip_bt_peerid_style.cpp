/*! \file
    \brief Implementation of the \ref bt_peerid_style_t

- reference all the peerid identification style as found in 
  http://wiki.theory.org/BitTorrentSpecification#peer_id
- to have so many identifications style shows how well their authors
  collaborate with each other...

*/

/* system include */
/* local include */
#include "neoip_bt_peerid_style.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_peerid_style_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerid_style_t	, AZUREUS	, "AZUREUS")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerid_style_t	, SHADOW	, "SHADOW")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerid_style_t	, MAINLINE	, "MAINLINE")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerid_style_t	, XBT		, "XBT")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerid_style_t	, OPERA		, "OPERA")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerid_style_t	, MLDONKEY	, "MLDONKEY")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_peerid_style_t	, BITONWHEELS	, "BITONWHEELS")
NEOIP_STRTYPE_DEFINITION_END(bt_peerid_style_t)

NEOIP_NAMESPACE_END





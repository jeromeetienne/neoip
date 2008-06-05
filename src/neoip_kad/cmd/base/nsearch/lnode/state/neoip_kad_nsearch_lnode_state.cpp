/*! \file
    \brief Implementation of the \ref kad_nsearch_lnode_state_t

*/

/* system include */
/* local include */
#include "neoip_kad_nsearch_lnode_state.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(kad_nsearch_lnode_state_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(kad_nsearch_lnode_state_t	, QUERYING	, "QUERYING")
NEOIP_STRTYPE_DEFINITION_ITEM(kad_nsearch_lnode_state_t	, SUCCEED	, "SUCCEED")
NEOIP_STRTYPE_DEFINITION_ITEM(kad_nsearch_lnode_state_t	, FAILED	, "FAILED")
NEOIP_STRTYPE_DEFINITION_END(kad_nsearch_lnode_state_t)

NEOIP_NAMESPACE_END







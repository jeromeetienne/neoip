/*! \file
    \brief Implementation of the \ref bt_ezswarm_state_t
    
\par Brief Description
bt_ezswarm_state_t describes the current state of a bt_ezswarm_t.
- it uses ERRTYPE and not STRTYPE to allow to put a explaination about why
  this state has been reached
  - this is usefull for the bt_ezswarm_state_t::ERROR

*/

/* system include */
/* local include */
#include "neoip_bt_ezswarm_state.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DEFINITION_START(bt_ezswarm_state_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_ezswarm_state_t	, ALLOC		, "ALLOC")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_ezswarm_state_t	, CHECK		, "CHECK")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_ezswarm_state_t	, SHARE		, "SHARE")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_ezswarm_state_t	, STOPPING	, "STOPPING")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_ezswarm_state_t	, STOPPED	, "STOPPED")
NEOIP_ERRTYPE_DEFINITION_ITEM(bt_ezswarm_state_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_END(bt_ezswarm_state_t)

NEOIP_NAMESPACE_END





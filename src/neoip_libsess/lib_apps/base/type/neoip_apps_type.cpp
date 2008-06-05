/*! \file
    \brief Implementation of the \ref apps_type_t

\par Brief description
It give the type of 'start' for an application
- apps_type_t::SYS_BOOT is for application which are daemon at the system level 
  - aka are likely automatically started when the system boot
- apps_type_t::USR_BOOT is for application which are daemon at the user level 
  - aka are likely automatically started when the user login and stopped when 
    it logout
- apps_type_t::USR_TEMP is for temporary application, aka no automatic startup
  - only explicitly started and stopped by the user

*/

/* system include */
/* local include */
#include "neoip_apps_type.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(apps_type_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(apps_type_t	, SYS_BOOT	, "SYS_BOOT")
NEOIP_STRTYPE_DEFINITION_ITEM(apps_type_t	, USR_BOOT	, "USR_BOOT")
NEOIP_STRTYPE_DEFINITION_ITEM(apps_type_t	, USR_TEMP	, "USR_TEMP")
NEOIP_STRTYPE_DEFINITION_END(apps_type_t)

NEOIP_NAMESPACE_END





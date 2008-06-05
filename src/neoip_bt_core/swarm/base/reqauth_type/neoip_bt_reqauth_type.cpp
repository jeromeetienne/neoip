/*! \file
    \brief Implementation of the \ref bt_reqauth_type_t
*/

/* system include */
/* local include */
#include "neoip_bt_reqauth_type.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(bt_reqauth_type_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(bt_reqauth_type_t	, DENY		, "DENY")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_reqauth_type_t	, COOP		, "COOP")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_reqauth_type_t	, HOPE		, "HOPE")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_reqauth_type_t	, GIVE		, "GIVE")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_reqauth_type_t	, FSTART	, "FSTART")
NEOIP_STRTYPE_DEFINITION_ITEM(bt_reqauth_type_t	, IDLE		, "IDLE")
NEOIP_STRTYPE_DEFINITION_END(bt_reqauth_type_t)

NEOIP_NAMESPACE_END





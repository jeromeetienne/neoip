/*! \file
    \brief Header of the \ref skey_auth_algo_t

*/

/* system include */
/* local include */
#include "neoip_skey_auth_algo.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(skey_auth_algo_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, SHA1		, "SHA1")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, RMD160	, "RMD160")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, MD5		, "MD5")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, MD4		, "MD4")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, MD2		, "MD2")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, TIGER		, "TIGER")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, HAVAL		, "HAVAL")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, SHA256	, "SHA256")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, SHA384	, "SHA384")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, SHA512	, "SHA512")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_auth_algo_t	, CRC32		, "CRC32")
NEOIP_STRTYPE_DEFINITION_END(skey_auth_algo_t)

NEOIP_NAMESPACE_END


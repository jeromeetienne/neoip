/*! \file
    \brief Header of the \ref skey_ciph_algo_t

*/

/* system include */
/* local include */
#include "neoip_skey_ciph_algo.hpp"


NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(skey_ciph_algo_t	, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, DES		, "DES")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, EDE_3DES	, "EDE_3DES")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, CAST5		, "CAST5")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, BLOWFISH	, "BLOWFISH")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, TWOFISH128	, "TWOFISH128")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, TWOFISH256	, "TWOFISH256")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, AES		, "AES")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, AES192	, "AES192")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, AES256	, "AES256")
NEOIP_STRTYPE_DEFINITION_ITEM(skey_ciph_algo_t	, ARCFOUR	, "ARCFOUR")
NEOIP_STRTYPE_DEFINITION_END(skey_ciph_algo_t)

NEOIP_NAMESPACE_END


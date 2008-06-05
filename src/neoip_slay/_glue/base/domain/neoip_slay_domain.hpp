/*! \file
    \brief Header of the \ref slay_domain_t

*/


#ifndef __NEOIP_SLAY_DOMAIN_HPP__ 
#define __NEOIP_SLAY_DOMAIN_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(slay_domain_t	, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(slay_domain_t	, TLS)
NEOIP_STRTYPE_DECLARATION_ITEM(slay_domain_t	, BTJAMSTD)
NEOIP_STRTYPE_DECLARATION_ITEM(slay_domain_t	, BTJAMRC4)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(slay_domain_t)
	bool is_tls()		const throw() { return get_value() == TLS;	}
	bool is_btjamstd()	const throw() { return get_value() == BTJAMSTD;	}
	bool is_btjamrc4()	const throw() { return get_value() == BTJAMRC4;	}
NEOIP_STRTYPE_DECLARATION_END(slay_domain_t	, uint8_t)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_DOMAIN_HPP__  */






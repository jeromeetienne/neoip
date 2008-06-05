/*! \file
    \brief Header of the \ref pktcomp_err_t

*/


#ifndef __NEOIP_PKTCOMP_ERR_HPP__ 
#define __NEOIP_PKTCOMP_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DECLARATION_START(pktcomp_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(pktcomp_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(pktcomp_err_t	, WOULD_EXPAND)
NEOIP_ERRTYPE_DECLARATION_ITEM(pktcomp_err_t	, CANT_DECOMPRESS)
NEOIP_ERRTYPE_DECLARATION_ITEM(pktcomp_err_t	, BOGUS_PKT)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(pktcomp_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(pktcomp_err_t)


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_PKTCOMP_ERR_HPP__  */



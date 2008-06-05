/*! \file
    \brief Header of the \ref netif_err_t

*/


#ifndef __NEOIP_NETIF_ERR_HPP__ 
#define __NEOIP_NETIF_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DECLARATION_START(netif_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(netif_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(netif_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(netif_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(netif_err_t)


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_NETIF_ERR_HPP__  */




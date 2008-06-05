/*! \file
    \brief Header of the packet type of the router
    

*/


#ifndef __NEOIP_KAD_NSEARCH_LNODE_STATE_HPP__ 
#define __NEOIP_KAD_NSEARCH_LNODE_STATE_HPP__ 
/* system include */
/* local include */
#include "neoip_strtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DECLARATION_START(kad_nsearch_lnode_state_t, 0)
NEOIP_STRTYPE_DECLARATION_ITEM(kad_nsearch_lnode_state_t	, QUERYING)
NEOIP_STRTYPE_DECLARATION_ITEM(kad_nsearch_lnode_state_t	, SUCCEED)
NEOIP_STRTYPE_DECLARATION_ITEM(kad_nsearch_lnode_state_t	, FAILED)
NEOIP_STRTYPE_DECLARATION_ITEM_LAST(kad_nsearch_lnode_state_t)
NEOIP_STRTYPE_DECLARATION_END(kad_nsearch_lnode_state_t	, uint8_t )

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_NSEARCH_LNODE_STATE_HPP__  */




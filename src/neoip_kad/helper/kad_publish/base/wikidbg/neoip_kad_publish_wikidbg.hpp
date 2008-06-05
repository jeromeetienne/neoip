/*! \file
    \brief Declaration of the kad_publish_wikidbg_t

*/


#ifndef __NEOIP_KAD_PUBLISH_WIKIDBG_HPP__ 
#define __NEOIP_KAD_PUBLISH_WIKIDBG_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

// list of include which are almost always needed when using this one
#include "neoip_wikidbg_obj.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class wikidbg_keyword_db_t;

// declaration of the wikidbg_init_cb_t for kad_publish_t
void kad_publish_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw();


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_PUBLISH_WIKIDBG_HPP__  */




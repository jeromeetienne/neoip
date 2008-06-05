/*! \file
    \brief Header of the wikidbg_init_db_t class

- FIXME this file has a unrepresentative name

*/


#ifndef __NEOIP_WIKIDBG_INIT_CB_HPP__ 
#define __NEOIP_WIKIDBG_INIT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_wikidbg_keyword_db.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

class wikidbg_keyword_db_t;
/** \brief The callback type to define the keyword specific to a given type
 */
typedef void (*wikidbg_init_cb_t)(wikidbg_keyword_db_t &keyword_db);


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_WIKIDBG_INIT_CB_HPP__  */




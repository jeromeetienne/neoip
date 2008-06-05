/*! \file
    \brief Declaration of the oload_httpo_ctrl_wikidbg_t

*/


#ifndef __NEOIP_OLOAD_HTTPO_CTRL_WIKIDBG_HPP__ 
#define __NEOIP_OLOAD_HTTPO_CTRL_WIKIDBG_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

// list of include which are almost always needed when using this one
#include "neoip_wikidbg_obj.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class wikidbg_keyword_db_t;

// declaration of the wikidbg_init_cb_t for oload_httpo_ctrl_t
void oload_httpo_ctrl_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw();


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_HTTPO_CTRL_WIKIDBG_HPP__  */




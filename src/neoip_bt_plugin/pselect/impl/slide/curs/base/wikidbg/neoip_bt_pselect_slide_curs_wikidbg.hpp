/*! \file
    \brief Declaration of the bt_pselect_slide_curs_wikidbg_t

*/


#ifndef __NEOIP_BT_PSELECT_SLIDE_CURS_WIKIDBG_HPP__ 
#define __NEOIP_BT_PSELECT_SLIDE_CURS_WIKIDBG_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

// list of include which are almost always needed when using this one
#include "neoip_wikidbg_obj.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class wikidbg_keyword_db_t;

// declaration of the wikidbg_init_cb_t for bt_herr_t
void bt_pselect_slide_curs_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw();


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PSELECT_SLIDE_CURS_WIKIDBG_HPP__  */




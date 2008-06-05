/*! \file
    \brief Definition of the \ref nipmem_tracker_http_t class

\par Brief description
nipmem_tracker_http_t is just a placeholder for the nipmem_tracker_t wikidbg.
the wikidbg is not directly on nipmem_tracker_t because it would create 
circular dependancy in the .hpp and in the libsess initialization phase.

\par Misc stuff from the past :)
- to provide option in the web page to filter/sort the entry
- display statistic on the tracked zone
- use some garbage collection technique to discover the chained allocation
  - e.g. object A has been allocated and contained a pointer on object B
  - so object A uses the size of object B too
  - selfsize = object A
  - subsize = object B
  - totsize = selfsize + subsize
- TODO to remove the nested class

*/

/* system include */
/* local include */
#include "neoip_nipmem_tracker_http.hpp"
#include "neoip_nipmem_tracker.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nipmem_tracker_http_t::nipmem_tracker_http_t()	throw()
{
	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/nipmem_tracker_" + OSTREAMSTR(this));
}

/** \brief Destructor
 */
nipmem_tracker_http_t::~nipmem_tracker_http_t()					throw()
{
}


NEOIP_NAMESPACE_END






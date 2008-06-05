/*! \file
    \brief Declaration of the \ref ndiag_watch_layer_t

*/


#ifndef __NEOIP_NDIAG_WATCH_INIT_HPP__
#define __NEOIP_NDIAG_WATCH_INIT_HPP__

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class ndiag_watch_t;

bool		ndiag_watch_init()	throw();
ndiag_watch_t *	ndiag_watch_get()	throw();
bool		ndiag_watch_deinit()	throw();

NEOIP_NAMESPACE_END

#endif // __NEOIP_NDIAG_WATCH_INIT_HPP__ 




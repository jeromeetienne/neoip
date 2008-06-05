/*! \file
    \brief Declaration of the \ref lib_session_t

*/


#ifndef __NEOIP_LIB_SESSION_INIT_HPP__
#define __NEOIP_LIB_SESSION_INIT_HPP__

/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class lib_session_t;

bool		lib_session_init()	throw();
lib_session_t *	lib_session_get()	throw();
bool		lib_session_deinit()	throw();

NEOIP_NAMESPACE_END

#endif // __NEOIP_LIB_SESSION_INIT_HPP__ 




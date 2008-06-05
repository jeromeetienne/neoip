/*! \file
    \brief Declaration of the \ref compress_layer_t

*/


#ifndef __NEOIP_COMPRESS_LAYER_INIT_HPP__
#define __NEOIP_COMPRESS_LAYER_INIT_HPP__

/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class compress_layer_t;

bool			compress_layer_init()	throw();
compress_layer_t *	compress_layer_get()	throw();
bool			compress_layer_deinit()	throw();

NEOIP_NAMESPACE_END

#endif // __NEOIP_COMPRESS_LAYER_INIT_HPP__ 




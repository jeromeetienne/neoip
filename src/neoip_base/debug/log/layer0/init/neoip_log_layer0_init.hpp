/*! \file
    \brief Declaration of the \ref log_layer_t

*/


#ifndef __NEOIP_LOG_LAYER_INIT_HPP__
#define __NEOIP_LOG_LAYER_INIT_HPP__

/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class log_layer_t;

bool		log_layer_init()	throw();
log_layer_t *	log_layer_get()		throw();
bool		log_layer_deinit()	throw();

NEOIP_NAMESPACE_END

#endif // __NEOIP_LOG_LAYER_INIT_HPP__ 




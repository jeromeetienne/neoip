/*! \file
    \brief Declaration of the \ref udp_layer_t

*/


#ifndef __NEOIP_UDP_LAYER_INIT_HPP__
#define __NEOIP_UDP_LAYER_INIT_HPP__

/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class udp_layer_t;

bool		udp_layer_init()	throw();
udp_layer_t *	udp_layer_get()		throw();
bool		udp_layer_deinit()	throw();

NEOIP_NAMESPACE_END

#endif // __NEOIP_UDP_LAYER_INIT_HPP__ 




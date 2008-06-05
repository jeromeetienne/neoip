/*! \file
    \brief Declaration of the \ref socket_layer_ntlay_t

*/


#ifndef __NEOIP_SOCKET_LAYER_NTLAY_INIT_HPP__
#define __NEOIP_SOCKET_LAYER_NTLAY_INIT_HPP__

/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class socket_layer_ntlay_t;

bool			socket_layer_ntlay_init()		throw();
socket_layer_ntlay_t *	socket_layer_ntlay_get()		throw();
bool			socket_layer_ntlay_deinit()	throw();

NEOIP_NAMESPACE_END

#endif // __NEOIP_SOCKET_LAYER_NTLAY_INIT_HPP__ 




/*! \file
    \brief Declaration of the \ref socket_layer_udp_t

*/

/* system include */
/* local include */
#include "neoip_socket_layer_udp_init.hpp"
#include "neoip_socket_layer_udp.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of the layer_udp pointer and counter
static socket_layer_udp_t *	socket_layer_udp_ptr		= NULL;
static int			socket_layer_udp_init_counter	= 0;

/** \brief init the socket_layer_udp_t
 */
bool	socket_layer_udp_init()	throw()
{
	// increase the init_counter
	socket_layer_udp_init_counter++;
	// if the layer_udp was already initialize, return no error
	if( socket_layer_udp_init_counter > 1 )	return false;
	// allocate the layer_udp
	socket_layer_udp_ptr	= nipmem_new socket_layer_udp_t();
	return false;
}

/** \brief Get the socket_layer_udp_t
 */
socket_layer_udp_t *socket_layer_udp_get()	throw()
{
	// sanity check - the socket_layer_udp_t MUST be already init
	DBG_ASSERT( socket_layer_udp_ptr );
	// return the socket_layer_udp
	return socket_layer_udp_ptr;
}

/** \brief deinit the socket_layer_udp_t
 */
bool	socket_layer_udp_deinit()	throw()
{
	// decrease the init_counter
	socket_layer_udp_init_counter--;
	// sanity check - detect the deinit more time than init
	DBG_ASSERT( socket_layer_udp_init_counter >= 0 );
	// if the layer_udp was already initialize, return no error
	if( socket_layer_udp_init_counter > 0 )	return false;
	// deallocate the layer_udp
	nipmem_delete	socket_layer_udp_ptr;
	socket_layer_udp_ptr	= NULL;
	return false;
}

NEOIP_NAMESPACE_END



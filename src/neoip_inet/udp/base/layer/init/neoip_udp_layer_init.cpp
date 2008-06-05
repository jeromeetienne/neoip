/*! \file
    \brief Declaration of the \ref udp_layer_t

\par Possible improvement
- it likely is a good idea to get all the layer init as a template/define stuff
*/

/* system include */
/* local include */
#include "neoip_udp_layer_init.hpp"
#include "neoip_udp_layer.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of the layer_udp pointer and counter
static udp_layer_t *	udp_layer_ptr		= NULL;
static int		udp_layer_init_counter	= 0;

/** \brief init the udp_layer_t
 */
bool	udp_layer_init()	throw()
{
	// increase the init_counter
	udp_layer_init_counter++;
	// if the layer_udp was already initialize, return no error
	if( udp_layer_init_counter > 1 )	return false;
	// allocate the layer_udp
	udp_layer_ptr	= nipmem_new udp_layer_t();
	return false;
}

/** \brief Get the udp_layer_t
 */
udp_layer_t *udp_layer_get()	throw()
{
	// sanity check - the udp_layer_t MUST be already init
	DBG_ASSERT( udp_layer_ptr );
	// return the udp_layer
	return udp_layer_ptr;
}

/** \brief deinit the udp_layer_t
 */
bool	udp_layer_deinit()	throw()
{
	// decrease the init_counter
	udp_layer_init_counter--;
	// sanity check - detect the deinit more time than init
	DBG_ASSERT( udp_layer_init_counter >= 0 );
	// if the layer_udp was already initialize, return no error
	if( udp_layer_init_counter > 0 )	return false;
	// deallocate the layer_udp
	nipmem_delete	udp_layer_ptr;
	udp_layer_ptr	= NULL;
	return false;
}

NEOIP_NAMESPACE_END



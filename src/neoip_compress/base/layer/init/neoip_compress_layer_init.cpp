/*! \file
    \brief Declaration of the \ref compress_layer_t

*/

/* system include */
/* local include */
#include "neoip_compress_layer_init.hpp"
#include "neoip_compress_layer.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of the layer pointer and counter
static compress_layer_t *	compress_layer_ptr		= NULL;
static int			compress_layer_init_counter	= 0;

/** \brief init the compress_layer_t
 */
bool	compress_layer_init()	throw()
{
	// increase the init_counter
	compress_layer_init_counter++;
	// if the layer was already initialize, return no error
	if( compress_layer_init_counter > 1 )	return false;
	// allocate the layer
	// TODO to change by using neoip mem lib
	compress_layer_ptr	= new compress_layer_t();
	return false;
}

/** \brief Get the compress_layer_t
 */
compress_layer_t *compress_layer_get()	throw()
{
	// sanity check - the compress_layer_t MUST be already init
	DBG_ASSERT( compress_layer_ptr );
	// return the compress_layer
	return compress_layer_ptr;
}

/** \brief deinit the compress_layer_t
 */
bool	compress_layer_deinit()	throw()
{
	// decrease the init_counter
	compress_layer_init_counter--;
	// sanity check - detect the deinit more time than init
	DBG_ASSERT( compress_layer_init_counter >= 0 );
	// if the layer was already initialize, return no error
	if( compress_layer_init_counter > 0 )	return false;
	// deallocate the layer
	// TODO to change by using neoip mem lib
	delete	compress_layer_ptr;
	compress_layer_ptr	= NULL;
	return false;
}

NEOIP_NAMESPACE_END



/*! \file
    \brief Declaration of the \ref log_layer_t

*/

/* system include */
/* local include */
#include "neoip_log_layer_init.hpp"
#include "neoip_log_layer.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of the layer pointer and counter
static log_layer_t *	log_layer_ptr		= NULL;
static int		log_layer_init_counter	= 0;

/** \brief init the log_layer_t
 */
bool	log_layer_init()	throw()
{
	// increase the init_counter
	log_layer_init_counter++;
	// if the layer was already initialize, return no error
	if( log_layer_init_counter > 1 )	return false;
	// allocate the layer
	// TODO to change by using neoip mem lib
	log_layer_ptr	= new log_layer_t();
	return false;
}

/** \brief Get the log_layer_t
 */
log_layer_t *log_layer_get()	throw()
{
	// sanity check - the log_layer_t MUST be already init
	DBG_ASSERT( log_layer_ptr );
	// return the log_layer
	return log_layer_ptr;
}

/** \brief deinit the log_layer_t
 */
bool	log_layer_deinit()	throw()
{
	// decrease the init_counter
	log_layer_init_counter--;
	// sanity check - detect the deinit more time than init
	DBG_ASSERT( log_layer_init_counter >= 0 );
	// if the layer was already initialize, return no error
	if( log_layer_init_counter > 0 )	return false;
	// deallocate the layer
	// TODO to change by using neoip mem lib
	delete	log_layer_ptr;
	log_layer_ptr	= NULL;
	return false;
}

NEOIP_NAMESPACE_END



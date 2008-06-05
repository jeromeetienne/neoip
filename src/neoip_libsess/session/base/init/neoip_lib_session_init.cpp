/*! \file
    \brief Declaration of the \ref lib_session_t

*/

/* system include */
/* local include */
#include "neoip_lib_session_init.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of the layer pointer and counter
static lib_session_t *	lib_session_ptr		= NULL;
static int		lib_session_init_counter= 0;

/** \brief init the lib_session_t
 */
bool	lib_session_init()	throw()
{
	// increase the init_counter
	lib_session_init_counter++;
	// if the layer was already initialize, return no error
	if( lib_session_init_counter > 1 )	return false;
	// allocate the layer
	lib_session_ptr	= new lib_session_t();
	return false;
}

/** \brief Get the lib_session_t
 */
lib_session_t *lib_session_get()	throw()
{
	// sanity check - the lib_session_t MUST be already init
	DBG_ASSERT( lib_session_ptr );
	// return the lib_session
	return lib_session_ptr;
}

/** \brief deinit the lib_session_t
 */
bool	lib_session_deinit()	throw()
{
	// decrease the init_counter
	lib_session_init_counter--;
	// sanity check - detect the deinit more time than init
	DBG_ASSERT( lib_session_init_counter >= 0 );
	// if the layer was already initialize, return no error
	if( lib_session_init_counter > 0 )	return false;
	// deallocate the layer
	delete	lib_session_ptr;
	lib_session_ptr	= NULL;
	return false;
}

NEOIP_NAMESPACE_END



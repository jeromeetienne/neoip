/*! \file
    \brief Declaration of the \ref ndiag_watch_layer_t

\par Possible improvement
- it likely is a good idea to get all the layer init as a template/define stuff

*/

/* system include */
/* local include */
#include "neoip_ndiag_watch_init.hpp"
#include "neoip_ndiag_watch.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of the ndiag_watch pointer and counter
static ndiag_watch_t *	ndiag_watch_ptr		= NULL;
static int		ndiag_watch_init_counter= 0;

/** \brief init the ndiag_watch_layer_t
 */
bool	ndiag_watch_init()	throw()
{
	// increase the init_counter
	ndiag_watch_init_counter++;
	// if the ndiag_watch was already initialize, return no error
	if( ndiag_watch_init_counter > 1 )	return false;
	// allocate the ndiag_watch
	ndiag_watch_ptr	= nipmem_new ndiag_watch_t();
	// return no error
	return false;
}

/** \brief Get the ndiag_watch_layer_t
 */
ndiag_watch_t *ndiag_watch_get()	throw()
{
	// sanity check - the ndiag_watch_t MUST be already init
	DBG_ASSERT( ndiag_watch_ptr );
	// return the ndiag_watch_ptr
	return ndiag_watch_ptr;
}

/** \brief deinit the ndiag_watch_t
 */
bool	ndiag_watch_deinit()	throw()
{
	// decrease the init_counter
	ndiag_watch_init_counter--;
	// sanity check - detect the deinit more time than init
	DBG_ASSERT( ndiag_watch_init_counter >= 0 );
	// if the ndiag_watch_t was already initialize, return no error
	if( ndiag_watch_init_counter > 0 )	return false;
	// deallocate the ndiag_watch
	nipmem_zdelete	ndiag_watch_ptr;
	// return no error
	return false;
}

NEOIP_NAMESPACE_END



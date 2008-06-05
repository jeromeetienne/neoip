/*! \file
    \brief Class to handle the idleop_t

*/

/* system include */
/* local include */
#include "neoip_idleop.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

gboolean	idleop_idle_glib_cb(gpointer data);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
idleop_t::idleop_t()		throw()
{
	priority	= G_PRIORITY_DEFAULT_IDLE;
	glib_tagid	= 0;
}

/** \brief Destructor
 */
idleop_t::~idleop_t()		throw()
{
	// remove the source if it has been started
	if( glib_tagid )	g_source_remove( glib_tagid );
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the priority between 0 and 100 included
 */
idleop_t &	idleop_t::set_priority(size_t priority)	throw()
{
	// sanity check - the priority MUST be contained in [0,100]
	DBG_ASSERT( priority >= 0 && priority <= 100 );
	// copy the parameter
	this->priority	= priority + G_PRIORITY_DEFAULT_IDLE;
	// return the object itself
	return *this;
}

/** \brief Start the operation
 * 
 * @return false on success, true otherwise
 */
bool	idleop_t::start(idleop_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	
	// start the idle
	glib_tagid	= g_idle_add_full(priority, idleop_idle_glib_cb, this, NULL);
	if( glib_tagid == 0 )	return true;
                                          
	// return no error
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool idleop_t::notify_callback()	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_idleop_cb(userptr, *this);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback of g_idle_add() used by glib
 * 
 * - IMPORTANT it is running in the 'main' thread
 */
gboolean	idleop_idle_glib_cb(gpointer data)
{
	idleop_t *idleop	= (idleop_t *)data;
	// notify the callback
	bool	tokeep	= idleop->notify_callback();
	if( !tokeep )	return FALSE;
	// return tokeep
	return TRUE;
}
NEOIP_NAMESPACE_END






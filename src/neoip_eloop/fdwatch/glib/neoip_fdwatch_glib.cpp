/*! \file
    \brief Implementation of the fdwatch_t

*/

#ifdef USE_ELOOP_GLIB
/* system include */
#include <iostream>

/* local include */
#include "neoip_fdwatch_glib.hpp"
#include "neoip_errno.hpp"
#include "neoip_log.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of \ref fdwatch_cond_t constant
const fdwatch_cond_t	fdwatch_t::NONE		= fdwatch_cond_t(0);
const fdwatch_cond_t	fdwatch_t::INPUT	= fdwatch_cond_t(1 << 0);
const fdwatch_cond_t	fdwatch_t::OUTPUT	= fdwatch_cond_t(1 << 1);
const fdwatch_cond_t	fdwatch_t::ERROR	= fdwatch_cond_t(1 << 2);
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the fdwatch_t
 * 
 * - WARNING: after this function, the fd is owned here and closing this object
 *            the fd will be closed
 */
fdwatch_t::fdwatch_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");	
	// zero some fields
	callback	= NULL;
	userptr		= NULL;
	// init the io_channel
	glib_iochannel	= NULL;
	glib_iowatch	= 0;
}

/** \brief destructor of the fdwatch_t
 */
fdwatch_t::~fdwatch_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// close the glib_iowatch if needed
	if( glib_iowatch )	g_source_remove( glib_iowatch );
	// close the glib_iochannel if needed
	if( glib_iochannel )	g_io_channel_shutdown( glib_iochannel, 0, NULL );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 * 
 * - WARNING: this function tranfert the onwership of the fd to this function
 * - this function never fails, so it doesnt return any error
 * - the callback may be null, but if so, the new_cond MUST be fdwatch_t::NONE
 */
fdwatch_t &	fdwatch_t::start(int fd, const fdwatch_cond_t &new_cond, fdwatch_cb_t * callback
							, void *userptr)	throw()
{
	// sanity check - if the callback is NULL, new_cond MUST be NONE
	DBG_ASSERT( callback || new_cond == fdwatch_t::NONE );
	// copy the parameters
	this->callback	= callback;
	this->userptr	= userptr;	
	// init the io_channel
	glib_iochannel	= g_io_channel_unix_new( fd );
	DBG_ASSERT( glib_iochannel );
	g_io_channel_set_encoding(glib_iochannel, NULL, NULL);
	g_io_channel_set_buffered(glib_iochannel, FALSE);
	// init the fdwatch_cond_t
	cond(new_cond);
	// return the object itself
	return *this;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       query function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the fd monitored by this fdwatch_t 
 */
int fdwatch_t::get_fd() const throw()
{
	return g_io_channel_unix_get_fd( glib_iochannel );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       action function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Change the callback to notify the event on the fdwatch_t
 */
fdwatch_t &fdwatch_t::change_callback(fdwatch_cb_t * callback, void *userptr)	throw()
{
	// copy the parameters
	this->callback	= callback;
	this->userptr	= userptr;
	// return the object itself
	return *this;
}


/** \brief set the fdwatch_cond_t for this fdwatch_t
 */
fdwatch_t &fdwatch_t::cond(const fdwatch_cond_t &new_cond)			throw()
{
	// copy the new condition
	this->cur_cond	= new_cond;
	// close the previous fdwatch if needed
	if( glib_iowatch ){
		g_source_remove( glib_iowatch );
		glib_iowatch	 = 0;
	}
	// if there is no condition, do nothing
	if( cur_cond.is_null() )	return *this;

	// convert the fdwatch_cond_t to a GIOCondition for glib
	int	glib_cond = 0;
	if( cur_cond.is_input() )	glib_cond	|= G_IO_IN;
	if( cur_cond.is_output() )	glib_cond	|= G_IO_OUT;
	if( cur_cond.is_error() )	glib_cond	|= G_IO_ERR;
	// start the fdwatch
	glib_iowatch	= g_io_add_watch(glib_iochannel, static_cast<GIOCondition>(glib_cond)
							, fdwatch_glib_cb, this);
	DBG_ASSERT( glib_iowatch );
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	fdwatch_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// build the string to return
	oss        << "fd="	<< get_fd();
	oss << " " << "cond="	<< cond();
	// return the just built string
	return oss.str();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          fdwatch callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief called by glib when the fd has some conditions meet
 * 
 * - just a trick of c/c++ interaction as glib is in C
 */
gboolean fdwatch_glib_cb( GIOChannel *source, GIOCondition glib_cond, gpointer userptr )	throw()
{
	// get the caller object
	fdwatch_t *	fdwatch	= (fdwatch_t *)userptr;
	// convert the io condition from glib to fdwatch_cond_t
	fdwatch_cond_t	cond;
	if( glib_cond & G_IO_IN )	cond	|= fdwatch_t::INPUT;
	if( glib_cond & G_IO_OUT )	cond	|= fdwatch_t::OUTPUT;
	if( glib_cond & G_IO_ERR )	cond	|= fdwatch_t::ERROR;
	// pass the event to the proper c++ object
	if( fdwatch->notify_callback(cond) )	return TRUE;
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief handle the fdwatch event in c++
 */
bool	fdwatch_t::notify_callback(const fdwatch_cond_t &cond)			throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_fdwatch_cb(userptr, *this, cond);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END
#endif 



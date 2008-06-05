/*! \file
    \brief Implementation of the fdwatch_t

\par about the event.h
- this header file contains a lot of #define.
  - #define doesnt respect namespace and may interfere with my own code 
  - those interference are hard to debug
- thus to include <event.h> MUST NOT be done in my own .hpp in order limit the
  possible interference
  - the issue and the solution is the same in timeout_levt2_t and fdwatch_t
- there is a external_ctx_buf[84] in the timeout_levt2_t class to preallocate the
  data and thus avoid a dynamic memory allocation at every timeout_levt2_t
- there is a struct event pointer, called external_ctx
  - it is pointing on the external_ctx_buf
  - it is initialized in external_ctor
- NOTE: if the struct event is larger than the external_ctx_buf, an assert is 
  triggered.
*/

#ifdef USE_ELOOP_LEVT

/* system include */
#include <iostream>
#include <unistd.h>
/* local include */
#include "neoip_fdwatch_levt.hpp"
#include "neoip_errno.hpp"
#include "neoip_log.hpp"

#ifdef	_WIN32
#	include "neoip_inet_oswarp.hpp"
#endif


#include <event.h>	// NOTE: i put it last as it contains a lot of #define and i
			// want it to interfere with as few code as possible

NEOIP_NAMESPACE_BEGIN;


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
	fd		= -1;
	external_started= false;	
	// sanity check - the external_ctx_buf MUST be at least as long as struct event
	DBG_ASSERT( sizeof(struct event) <= sizeof(external_ctx_buf) );
	// set the external_ctx
	external_ctx	= (struct event *)external_ctx_buf;
}

/** \brief destructor of the fdwatch_t
 */
fdwatch_t::~fdwatch_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// if the external watch is started, stop it
	if( fd != -1 && external_is_started() )	external_stop();
	// if the fd is set, close it 
#ifndef	_WIN32
	if( fd != -1 )	close(fd);
#else
	if( fd != -1 )	inet_oswarp_t::close_fd(fd);
#endif
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
	this->fd	= fd;
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
	return fd;
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
	// log to debug
	KLOG_DBG("enter new_cond=" << new_cond);
	// if new_cond == cur_cond, return now
	// - this is an optimisation because some code does this a lot (e.g. tcp_full_t)
	if( cur_cond == new_cond )	return *this;
	// copy the new condition
	cur_cond	= new_cond;
	// close the previous fdwatch if needed
	if( external_is_started() )	external_stop();
	// if there is no condition, do nothing
	if( cur_cond.is_null() )	return *this;
	// start the new conditition
	external_start();
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          fdwatch callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called by libevent when the timeout expires
 * 
 * - just a trick of c/c++ interaction as libevent is in C
 */
void	external_neoip_fdwatch_levt_cb(int fd, short levt_event, void *userptr)
{
	// get the caller object
	fdwatch_t *	fdwatch	= (fdwatch_t *)userptr;
	// convert the io condition from glib to fdwatch_cond_t
	fdwatch_cond_t	cond;
	if( levt_event & EV_READ )	cond	|= fdwatch_t::INPUT;
	if( levt_event & EV_WRITE)	cond	|= fdwatch_t::OUTPUT;
	// TODO libevent has no EV_ERROR... sort this one out
	
	// log to debug
	KLOG_DBG("notify cond=" << cond);
	// pass the event to the proper c++ object
	fdwatch->notify_callback(cond);
}
/** \brief Start the external timer to expire after delay
 */
void	fdwatch_t::external_start()		throw()
{
	// sanity check - the external watch MUST NOT be started
	DBG_ASSERT( !external_is_started() );
	// convert the fdwatch_cond_t to a libevent one
	short	levt_cond	= 0;
	if( cur_cond.is_input() )	levt_cond |= EV_READ;
	if( cur_cond.is_output() )	levt_cond |= EV_WRITE;
	// use event_set() - NOTE: in anycase the event has EV_PERSIST
	event_set(external_ctx, fd, levt_cond | EV_PERSIST, external_neoip_fdwatch_levt_cb, this);	
	// use event_add() with no timer
	event_add(external_ctx, NULL);
	// mark the external as started
	external_started	= true;
}

/** \brief Delete the external timer
 */
void	fdwatch_t::external_stop()						throw()
{
	// sanity check - the external timer MUST be started
	DBG_ASSERT( external_is_started() );
	// use event_del()
	event_del(external_ctx);
	// mark the external as NOT started
	external_started	= false;
}

/** \brief return true if the external timer is current started, false otherwise
 */
bool	fdwatch_t::external_is_started()					const throw()
{
	return external_started;	
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



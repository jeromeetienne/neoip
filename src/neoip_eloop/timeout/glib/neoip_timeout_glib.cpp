/*! \file
    \brief Implementation of the \ref timeout_t

- TODO clean up before porting it to libevent
  - make a clean separation between the generic code and the one specific to glib
  - DONE remove the nullifying() zeroing() stuff this is old and poortly coded stuff
    - removed the zeroing()
    - i think i can remove the nullifying()
  - inline the basic getter function
  - proper order of the function in the .cpp
  - renaming function if needed
  - document it well
    - especially the weird case, like is_running() with non expirable 
    - all what it is supposed to do, aka iterative timer


- TODO to rename is_running() as is_started()
  - more explicit on the meaning, no more ambiguity with the infinit delay_t

\par Brief Description
\ref timeout_t is an iterative timer. once started (via timeout_t::start()), it will
run forever until it is deleted or stopped (via timeout_t::stop()).
- it is allowed to change the period of the timeout_t during the expiration callback
- it is allowed to stop a timeout_t during the expiration callback
- it is allowed to set the period to delay_t::ALWAYS or delay_t::NEVER
  - in this case, the timeout_t is declared running (may be tested via timeout_t::is_running())
    but will never expire.
  - get_expire_delay() will return delay_t::ALWAYS or repespectivly delay_t::NEVER

*/

#ifdef USE_ELOOP_GLIB
/* system include */
#include <iostream>

/* local include */
#include "neoip_timeout_glib.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
timeout_t::timeout_t()		throw()
{
	glib_timeout	= 0;
	callback	= NULL;
}

/** \brief Destructor
 */ 
timeout_t::~timeout_t()		throw()
{
	// if the timeout isnt running, do nothing
	if( glib_timeout ){
		// stop glib_timeout and mark it unused
		g_source_remove( glib_timeout );
		glib_timeout = 0;
	}
}
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief start the timeout
 * 
 * @param period	the period delay between timeout notification
 *                  	- delay_t::NEVER is allowed but not delay_t::ALWAYS
 */
void	timeout_t::start(const delay_t &period, timeout_cb_t * callback, void *userptr) throw()
{
	// sanity check - period MUST NOT be null
	DBG_ASSERT( !period.is_null() );
	DBG_ASSERT( callback );
	// stop the timeout if it is running
	if( glib_timeout ){
		// stop glib_timeout and mark it unused
		g_source_remove( glib_timeout );
		glib_timeout = 0;
	}
	// set the local variable
	this->period		= period;
	this->callback		= callback;
	this->userptr		= userptr;
	// start the glib timeout
	launch_glib_timeout(period);
}

/** \brief stop the timeout
 * 
 * - it is acceptable to call stop() on a already stopped timeout
 * - it is allowed to call this function during a callback
 */
void	timeout_t::stop()			throw()
{
	// if the timeout isnt running, do nothing
	if( glib_timeout ){
		// stop glib_timeout and mark it unused
		g_source_remove( glib_timeout );
		glib_timeout = 0;
	}
	// mark next_expire_date as null
	next_expire_date = delay_t();
}

/** \brief return true if the timeout is currently running, false otherwise
 */
bool	timeout_t::is_running()		const throw()
{
	// sanity check - the object MUST NOT be null
	if( is_null() )				return false;
	if( next_expire_date.is_null() )	return false;
	// if all the previous tests passed, return true
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int	timeout_t::compare(const timeout_t & other)	const throw()
{
	// sanity check - the object MUST NOT be null
	DBG_ASSERT( is_null() == false );
	// comparing timeout_t is equivalent to comparing their expiration date
	return next_expire_date.compare(other.next_expire_date);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	timeout_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )		return "null";
	// build the string
	oss << next_expire_date;
	// return just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       utility function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the delay in which this timeout_t will expire
 * 
 * - if timeout_t is not running, return a null delay_t
 */
delay_t timeout_t::get_expire_delay()			const throw()
{
	// sanity check - the object MUST NOT be null
	DBG_ASSERT( is_null() == false );
	// if the timeout isnt running, return a null delay_t
	if( is_running() == false )	return	delay_t();
	// sanity check - expire_date MUST NOT be null
	DBG_ASSERT( !next_expire_date.is_null() );
	// if the next expire date is special, return it
	if( next_expire_date.is_special() )	return next_expire_date;
	// return the delta between the present and the expire_date
	return next_expire_date - date_t::present();
}

/** \brief change the period used by this timeout_t
 * 
 * - it is allowed to call this function during a callback
 */
void	timeout_t::change_period(const delay_t &period)	throw()
{
	// sanity check - the object MUST NOT be null
	DBG_ASSERT( !is_null() );
	// change the period value	
	this->period	= period;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      main notification callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	timeout_t::notify_callback()				throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_timeout_expire_cb( userptr, *this );
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       timeout expiration
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called by glib_timeout_add when the timeout expires
 * 
 * - just a trick of c/c++ interaction as glib is in C
 */
gboolean neoip_timeout_glib_expire(gpointer userptr)
{
	timeout_t	*timeout = (timeout_t *)userptr;
	timeout->cpp_expired_cb();
	// always return FALSE as the timer is never kept
	return FALSE;
}

/** \brief handle the timeout expiration (in c++, the c part is handled by neoip_timeout_glib_expire)
 */
void timeout_t::cpp_expired_cb()				throw()
{
	// logging for debug
	KLOG_DBG("enter callback="<< (callback ? neoip_cpp_typename(*callback) : "NULL")
			<< " . glib_timeout=" << glib_timeout << " this=" << this);
	// stop the glib timeout and mark it unused
#if 0
	// TODO is this g_source_remove usefull ?
//	g_source_remove( glib_timeout );
#endif
	glib_timeout	= 0;

	// NOTE: this callback may stop the timeout or delete it
	// - to stop the timeout, the callback MUST call timeout_t::stop()
	// - it return true if the timeout has been deleted, so the object 
	//   fields MUST NOT be used after that.
	bool	tokeep = notify_callback();
	if( !tokeep )	return;

	// if the timer is still running and glib_timeout is still 0, relaunch the timer
	// - here "still running" = "has not been stopped by the callback"
	// - glib_timeout may have been changed during the callback if timeout_t::start() has been called
	if( is_running() && glib_timeout == 0 )	launch_glib_timeout(period);
}

/** \brief launch the glib timeout if needed
 * 
 * - if delay is null, dont start the timeout
 */
void	timeout_t::launch_glib_timeout(const delay_t &delay)	throw()
{
	// sanity check - the glib timeout MUST NOT be running here
	DBG_ASSERT( glib_timeout == 0 );
	// if the delay is null, do notihing
	if( delay.is_null() ){
		// set the next_expire_date to null delay_t
		next_expire_date= delay_t();
		return;
	}
	// if the delay is special (aka NEVER or ALWAYS), set next_expire_date but not the glib timeout
	if( delay.is_special() ){
		next_expire_date= delay;
		return;
	}
	// if delay is NOT null or special, compute the actual date
	next_expire_date= date_t::present() + delay;
	// set glib timeout according to the delay
	glib_timeout	= g_timeout_add(delay.to_msec_32bit(), neoip_timeout_glib_expire, this);
}

NEOIP_NAMESPACE_END
#endif




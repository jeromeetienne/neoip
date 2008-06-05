/*! \file
    \brief Implementation of the \ref timeout_t

\par about the event.h
- this header file contains a lot of #define.
  - #define doesnt respect namespace and may interfere with my own code 
  - those interference are hard to debug
- thus to include <event.h> MUST NOT be done in my own .hpp in order limit the
  possible interference
  - the issue and the solution is the same in timeout_t and fdwatch_levt_t
- there is a external_ctx_buf[84] in the timeout_t class to preallocate the
  data and thus avoid a dynamic memory allocation at every timeout_t
- there is a struct event pointer, called external_ctx
  - it is pointing on the external_ctx_buf
  - it is initialized in external_ctor
- NOTE: if the struct event is larger than the external_ctx_buf, an assert is 
  triggered.

*/

#ifdef USE_ELOOP_LEVT
/* system include */
#include <iostream>
/* local include */
#include "neoip_timeout_levt.hpp"
#include "neoip_log.hpp"

#include <event.h>	// NOTE: i put it last as it contains a lot of #define and i
			// want it to interfere with as few code as possible

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
	// zero the callback to detect the null case
	callback	= NULL;
	// init the external timer
	external_ctor();
}

/** \brief Destructor
 */ 
timeout_t::~timeout_t()		throw()
{
	// if the external timer is started, stop it
	if( external_is_started() )	external_stop();
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
	// if the external timer is started, stop it
	if( external_is_started() )	external_stop();
	// set the local variable
	this->period		= period;
	this->callback		= callback;
	this->userptr		= userptr;
	// start the levt timeout
	launch_timerext(period);
}

/** \brief stop the timeout
 * 
 * - it is acceptable to call stop() on a already stopped timeout
 * - it is allowed to call this function during a callback
 */
void	timeout_t::stop()			throw()
{
	// if the external timer is started, stop it
	if( external_is_started() )	external_stop();
	// mark next_expire_date as null
	next_expire_date = delay_t();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the timeout is currently running, false otherwise
 */
bool	timeout_t::is_started()		const throw()
{
	// sanity check - the object MUST NOT be null
	if( is_null() )				return false;
	if( next_expire_date.is_null() )	return false;
	// if all the previous tests passed, return true
	return true;
}

/** \brief return the delay in which this timeout_t will expire
 * 
 * - if timeout_t is not running, return a null delay_t
 */
delay_t timeout_t::get_expire_delay()			const throw()
{
	// sanity check - the object MUST NOT be null
	DBG_ASSERT( is_null() == false );
	// if the timeout isnt running, return a null delay_t
	if( is_started() == false )	return	delay_t();
	// sanity check - expire_date MUST NOT be null
	DBG_ASSERT( !next_expire_date.is_null() );
	// if the next expire date is special, return it
	if( next_expire_date.is_special() )	return next_expire_date;
	// return the delta between the present and the expire_date
	return next_expire_date - date_t::present();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
//                       internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief launch the levt timeout if needed
 * 
 * - if delay is null, dont start the timeout
 */
void	timeout_t::launch_timerext(const delay_t &delay)	throw()
{
	// sanity check - the external timer MUST NOT be started
	DBG_ASSERT( !external_is_started() );
	// log to debug
	KLOG_DBG("enter delay=" << delay);
	// if the delay is null, do notihing
	if( delay.is_null() ){
		// set the next_expire_date to null delay_t
		next_expire_date= delay_t();
		return;
	}
	// if the delay is special (aka NEVER or ALWAYS), set next_expire_date but no external_start()
	if( delay.is_infinite() ){
		next_expire_date= delay;
		return;
	}
	// if delay is NOT null or special, compute the actual date
	next_expire_date= date_t::present() + delay;
	// start the external timer
	external_start(delay);
}

/** \brief handle the timeout expiration (in c++, the c part is handled by external_callback)
 */
void timeout_t::cpp_expired_cb()				throw()
{
	// logging for debug
	KLOG_DBG("enter");

	// NOTE: this callback may stop the timeout or delete it
	// - to stop the timeout, the callback MUST call timeout_t::stop()
	// - it return true if the timeout has been deleted, so the object 
	//   fields MUST NOT be used after that.
	bool	tokeep	= notify_callback();
	if( !tokeep )	return;

	// if the timeout_t is still started and external timer is not start, relaunch the external timer
	// - here "still started" = "has not been stopped by the callback"
	// - the external timer may have been changed during the callback if timeout_t::start() 
	//   has been called
	if( is_started() && !external_is_started() )	launch_timerext(period);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     interaction with external timer
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called by libevent when the timeout expires
 * 
 * - just a trick of c/c++ interaction as libevent is in C
 */
void	external_callback(int fd, short event, void *userptr)
{
	timeout_t	*timeout = (timeout_t *)userptr;
	timeout->cpp_expired_cb();
}

/** \brief Initialize the external timer
 */
void	timeout_t::external_ctor()						throw()
{
	// sanity check - the external_ctx_buf MUST be at least as long as struct event
	DBG_ASSERT( sizeof(struct event) <= sizeof(external_ctx_buf) );
	// set the external_ctx
	external_ctx	= (struct event *)external_ctx_buf;
	// use evtimer_set()
	evtimer_set(external_ctx, external_callback, this);	
}

/** \brief Start the external timer to expire after delay
 */
void	timeout_t::external_start(const delay_t &delay)			throw()
{
	struct	timeval	tv;
	// sanity check - the external timer MUST NOT be started
	DBG_ASSERT( !external_is_started() );	
	// convert delay into tv
	tv.tv_sec	= delay.to_msec_32bit() / 1000;
	tv.tv_usec	= (delay.to_msec_32bit() % 1000) * 1000;
	// log to debug
	KLOG_DBG("to_msec_32bit()=" << delay.to_msec_32bit());
	KLOG_DBG("tv_sec=" << tv.tv_sec << " tv_usec=" << tv.tv_usec);
	// use evtimer_add()
	evtimer_add(external_ctx, &tv);
}

/** \brief Delete the external timer
 */
void	timeout_t::external_stop()						throw()
{
	// sanity check - the external timer MUST be started
	DBG_ASSERT( external_is_started() );
	// use evtimer_del
	evtimer_del(external_ctx);
}

/** \brief return true if the external timer is current started, false otherwise
 */
bool	timeout_t::external_is_started()					const throw()
{
	// use evtimer_pending()
	return evtimer_pending(const_cast<struct event *>(external_ctx), NULL);	
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main comparison function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 objects (ala memcmp)
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
	bool tokeep = callback->neoip_timeout_expire_cb(userptr, *this);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END

#endif




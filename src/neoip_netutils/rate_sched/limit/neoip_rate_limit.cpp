/*! \file
    \brief Definition of the \ref rate_limit_t

\par Brief Description
- handle a given rate_limit_t attached to a rate_sched_t
- the rate_sched_t has a global max in rate_sched_t::rate_max()
- each rate_limit_t has a precedence (rate_prec_t) and an absolute maximum rate
- the absolute maximum rate is NEVER exceeded, no matter how much is currently
  in use. this is considered as a "hard limit"
  - a rate_limit_t NEVER allows more than the absrate_max()
- the precedence, on the other hand, indicates the 'share' of the rate_sched_t::max_rate()
  which is allowed to this rate_limit_t
  - if some rate_limit_t don't use all their allowed bandwidth, the remaining
    is reallocated to the others
    
\par Description of the 2 state
-# idle state: in which the rate_limit_t doesnt do anything but waiting for request
   - when idle, used_prec = 0
-# used state: this happen just after a request/notify couple, and last until a 
   timeout window_delay, once this timeout expire this becomes idle again, then
   the caller is notified of the expiration
   - when used, used_prec = maxi_prec

\par Description of Usage
-# the caller create a rate_limit_tmp_t variable - here called rate_tmp
   - this variable is used *internally* to pass information between the request and notify functions
-# the caller determine how much data are needed to request and put it in "request_len"
   - if it doesnt need any, no need to do the request/notify
   - it MAY be done but will result in the callback being notified in '0second'  
-# The caller request the amount to the rate_limit_t
   - size_t maxi_len	= rate_limit->data_request(request_len, rate_tmp);
   - the rate_limit_t will compute the amount of data allowed by this request
   - it is always <= request_len
-# the caller do its work with the maxi_len e.g. it write data in a socket
   -# int used_len = write(sockfd, buffer, maxi_len);
-# The caller notifies the used_len to the rate_limit_t
   - rate_limit->data_notify(used_len, rate_tmp);
-# after a data_notify(), the rate_limit is always be used and will remain this way
   until the callback is notified
   - during this used-state, the caller MAY NOT request any data on the rate_limit_t

\par About notifying 0 byte after a request
- description: if the caller request to some data (so before actually using/writing them)
  and then notify it has use 0byte
- there is 2 cases: (i) the request allow
  1. the request didnt allow any byte
     - e.g. the rate_limit_t::abs_maxrate() is 0
     - e.g. the rate_sched_t::maxrate() is 0
  2. the request allowed data, but the caller could not write them
     - e.g. writing on a socket, a data amount is request and allowed but the socket
       buffer is full and the write() doent write any data
- after both cases, the caller MUST notify the data usage
- how the notification reacts in those cases:
  1. if no data has been allowed by the request, no data will be allowed for a 
     given time, tunable by rate_limit_profile_t::frozen_window_delay()
     - this avoid to request data on a short loop and consume 100% cpu by 
       always requesting data and be always rejected.
     - during this time, the rate_limit_t is considered used
  2. if data has been allowed by the request, but the caller was unable to use 
     it. the notification will not trigger the timeout and the rate_limit_t will
     remain idle.
     - thus the caller wont be notified of the end of the used window.
     - so it is up to the caller to detect when it will be able to use the data
       - e.g. in a socket, this mean using like a select() to determine when
         it is ok to write in it.

*/

/* system include */
/* local include */
#include "neoip_rate_limit.hpp"
#include "neoip_rate_sched.hpp"
#include "neoip_rand.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
rate_limit_t::rate_limit_t()	throw()
{
	// zero some fields
	m_rate_sched	= NULL;
	// set some default value
	m_absrate_max	= std::numeric_limits<double>::max();
}

/** \brief Destructor
 */
rate_limit_t::~rate_limit_t()	throw()
{
	if( m_rate_sched ){
		// remove the local precendences from the precomputed sym
		rate_sched()->m_maxi_precsum	-= maxi_prec();
		rate_sched()->m_used_precsum	-= used_prec();
		// unlink this object from the pool if needed
		rate_sched()->item_unlink(this);
		// sanity check - rate_sched MUST be is_sane() at this moment
		DBG_ASSERT( rate_sched()->is_sane() );
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief set the profile
 */
rate_limit_t &	rate_limit_t::set_profile(const rate_limit_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == rate_err_t::OK );	
	// copy the parameter
	m_profile	= p_profile;
	// return the object itself
	return *this;
}

/** \brief Start the operation
 */
rate_err_t	rate_limit_t::start(rate_sched_t *p_rate_sched, const rate_prec_t &p_maxi_prec
					, rate_limit_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->m_rate_sched	= p_rate_sched;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the pool
	rate_sched()->item_dolink(this);
	// update the maxi_prec
	maxi_prec(p_maxi_prec);
	// start the used_prec as if it dont use anything
	used_prec(rate_prec_t(0));

	// sanity check - this object MUST be is_sane()
	DBG_ASSERT( is_sane() );
	// sanity check - rate_sched MUST be is_sane() at this moment
	DBG_ASSERT( rate_sched()->is_sane() );
		
	// return no error
	return rate_err_t::OK;
}

/** \brief Start the operation with a rate_limit_arg_t
 */
rate_err_t	rate_limit_t::start(const rate_limit_arg_t &arg, rate_limit_cb_t *callback
							, void *userptr)	throw()
{
	// sanity check - the rate_limit_arg_t MUST be valid
	DBG_ASSERT( arg.is_valid() );
	// if the "profile" arg is present, set it up
	if( arg.profile_present() )	set_profile(arg.profile());
	// if the "absrate_max" is present, set it up
	if( arg.absrate_max_present())	absrate_max(arg.absrate_max());
	// start the operation now
	return start(arg.rate_sched(), arg.rate_prec(), callback, userptr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if rate_limit_t is sane, false otherwise
 * 
 * - intended only for debug purpose 
 * - e.g. DBG_ASSERT( rate_limit.is_sane() );
 */
bool	rate_limit_t::is_sane()			const throw()
{
	if( is_used() && used_prec() != maxi_prec() ){
		KLOG_ERR("used_prec=" << used_prec() );
		KLOG_ERR("maxi_prec=" << maxi_prec() );
	}
	if( is_idle() && used_prec() != rate_prec_t(0) ){
		KLOG_ERR("used_prec=" << used_prec() );
		KLOG_ERR("maxi_prec=" << maxi_prec() );
	}

	// if is_used, used_prec() MUST be equal to maxi_prec()
	if( is_used() )	DBG_ASSERT( used_prec() == maxi_prec() );
	// if is_idle, used_prec() MUST be equal to 0
	if( is_idle() )	DBG_ASSERT( used_prec() == rate_prec_t(0) );
	// if this point is reached, it is considered sane, and return true
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update precedence
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set a new rate_prec_t to this rate_limit_t
 * 
 * - NOTE: it may be done at any moment during the life of the rate_limit_t object
 *   - this means even after the start()
 * - it has been designed to consume very little cpu
 */
void	rate_limit_t::maxi_prec(const rate_prec_t &new_prec)			throw()
{
	// sanity check - this object MUST be is_sane()
	DBG_ASSERT( is_sane() );
	// sanity check - rate_sched MUST be is_sane() at this moment
	DBG_ASSERT( rate_sched()->is_sane() );

	// if is_used(), update used_prec() too
	if( is_used() )	used_prec( new_prec );

	// remove the old rate_prec_t from the rate_sched()->maxi_precsum
	rate_sched()->m_maxi_precsum	-= maxi_prec();
	// update the rate_prec_t of this rate_limit_t
	m_maxi_prec			 = new_prec;
	// add the new rate_prec_t from the rate_sched()->maxi_precsum
	rate_sched()->m_maxi_precsum	+= maxi_prec();
	
	// sanity check - this object MUST be is_sane()
	DBG_ASSERT( is_sane() );
	// sanity check - rate_sched MUST be is_sane() at this moment
	DBG_ASSERT( rate_sched()->is_sane() );
}

/** \brief Update the used_prec
 * 
 * - this compute the used_prec and update the rate_sched()->used_precsum
 */
void	rate_limit_t::used_prec(const rate_prec_t &new_prec)		throw()
{
	// remove the old rate_prec_t from the rate_sched()->used_precsum
	rate_sched()->m_used_precsum	-= used_prec();
	// update the rate_prec_t of this rate_limit_t
	m_used_prec			 = new_prec;
	// add the new rate_prec_t from the rate_sched()->used_precsum
	rate_sched()->m_used_precsum	+= used_prec();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       timeout_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	rate_limit_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// log to debug
	KLOG_DBG("enter");

	// mark the used_prec to show it is no more used
	used_prec(rate_prec_t(0));
	
	// stop the inuse_timeout
	inuse_timeout.stop();

	// sanity check - at this point, it should be considered idle
	DBG_ASSERT( is_idle() );
	// sanity check - rate_sched MUST be is_sane() at this moment
	DBG_ASSERT( rate_sched()->is_sane() );

	// notify the caller that
	bool	tokeep	= notify_callback();
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief For the caller, to request data transmittion
 */
size_t	rate_limit_t::data_request(size_t request_len, rate_limit_tmp_t &limit_tmp)	const throw()
{
	// sanity check - the rate_limit_t MUST NOT be inuse
	DBG_ASSERT( !is_used() );
	// sanity check - this object MUST be is_sane()
	DBG_ASSERT( is_sane() );
	// sanity check - rate_sched MUST be is_sane() at this moment
	DBG_ASSERT( rate_sched()->is_sane() );
	// compute the ratio of precedence for this rate_limit_t
	rate_prec_t precsum	= rate_sched()->used_precsum() - used_prec() + maxi_prec();
	double	prec_ratio	= maxi_prec().to_double() / precsum.to_double();
	DBG_ASSERT( prec_ratio <= 1.0 );
	// compute the allowed rate
	double	allowed_rate	= rate_sched()->max_rate() * prec_ratio;
	// clamp the allowed_rate with the absrate_max()
	allowed_rate		= std::min(allowed_rate, absrate_max());

	// randomize the window_delay - it helps spread all the requests over time
	limit_tmp.window_delay	= neoip_rand_rate(profile().req_window_delay().to_sec_double(), profile().req_window_randrate());
	// compute the maxi_len from the allowed_rate and window_delay
	limit_tmp.maxi_len	= allowed_rate * limit_tmp.window_delay;
	// make sure maxi_len is castable in a size_t - as the function return a size_t
	limit_tmp.maxi_len	= std::min(limit_tmp.maxi_len, double(std::numeric_limits<size_t>::max()));

	// log to debug
	KLOG_DBG("request_len=" << request_len << " maxi_len=" << limit_tmp.maxi_len
					<< " size_t(maxi_len)="<< size_t(limit_tmp.maxi_len)
					<< " allowed_rate=" << allowed_rate
					<< " window_delay=" << limit_tmp.window_delay 
					<< " used_precsum=" << rate_sched()->used_precsum()
					<< " maxi_precsum=" << rate_sched()->maxi_precsum());
	// return the maxi_len
	return std::min(size_t(limit_tmp.maxi_len), request_len);
}

/** \brief For the caller to notify data actually transmitted
 * 
 * - NOTE: if used_len == 0, rate_limit_tmp_t MUST BE ignored
 */
void	rate_limit_t::data_notify(size_t used_len, const rate_limit_tmp_t &limit_tmp)	throw()
{
	// sanity check - the rate_limit_t MUST NOT be inused
	DBG_ASSERT( !is_used() );
	// sanity check - the used_len MUST be <= limit_tmp.maxi_len
	DBG_ASSERT( used_len <= size_t(limit_tmp.maxi_len) );
	// sanity check - this object MUST be is_sane()
	DBG_ASSERT( is_sane() );
	// sanity check - rate_sched MUST be is_sane() at this moment
	DBG_ASSERT( rate_sched()->is_sane() );
	// sanity check - at this point used_prec() MUST be 0
	DBG_ASSERT( used_prec() == rate_prec_t(0) );
	
 	// if rate_limit_t did not allow to write, do a predefined timeout
 	// - NOTE: it aims to wait a bit before retrying to check if it is allowed
	if( limit_tmp.maxi_len == 0 ){
		// set the used_prec to maxi_prec() for the duration of the inuse_timeout
		used_prec( maxi_prec() );
		// start the inuse_timeout	
		double	window_delay	= neoip_rand_rate(profile().frozen_window_delay().to_sec_double(), profile().frozen_window_randrate());
		delay_t	inuse_delay	= delay_t(uint64_t(window_delay*1000.0));
		inuse_timeout.start(inuse_delay, this, NULL);
		return;
	}
 
 	// if used_len == 0, do nothing (aka the caller will not receive a timeout)
 	// - it is up to the caller to retry later 
	// - TODO shouldnt this update the used_prec to 0 ?
	//   - YEP pretty sure it should be... maybe it already it tho
 	if( used_len == 0 )	return;
 
	// set the used_prec to maxi_prec() for the duration of the inuse window
	used_prec( maxi_prec() );
	// compute the effective window_delay 
	double	used_ratio	= double(used_len) / limit_tmp.maxi_len;
	double	window_delay	= limit_tmp.window_delay * used_ratio;
	delay_t	inuse_delay	= delay_t(uint64_t(window_delay*1000.0));

	// log to debug
	KLOG_DBG("used_prec="	<< used_prec() << " maxi_prec=" << maxi_prec()
				<< " used_len=" << used_len
				<< " inuse_delay=" << inuse_delay);
	// start the inuse_timeout
	inuse_timeout.start(inuse_delay, this, NULL);
	// sanity check - this object MUST be is_sane()
	DBG_ASSERT( is_sane() );
	// sanity check - rate_sched MUST be is_sane() at this moment
	DBG_ASSERT( rate_sched()->is_sane() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback 
 */
bool	rate_limit_t::notify_callback()		throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_rate_limit_cb(userptr, *this);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;





/*! \file
    \brief Implementation of the timer_probing_t

- TODO it may be good to handle the timer_policy differently
  - closer to the stl iterator
  - using the .begin() and .end()
  - the ++ and --
  - is_finished()
  - .total() the total amount of time delivered by this policy
    
*/

/* system include */
/* local include */
#include "neoip_timer_probing.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief the constructor
 */
timer_probing_t::timer_probing_t(const delay_t &delay_begin, const delay_t &delay_end
					, const delay_t &probe_period)		throw()
{
	// sanity check - delay_begin MUST be defined
	DBG_ASSERT( delay_begin.is_special() == false );
	DBG_ASSERT( delay_end.is_never() == false );
	
	// copy the parameters
	this->delay_begin	= delay_begin;
	this->delay_end		= delay_end;
	this->probe_period	= probe_period;
	
	// set delay_current
	delay_current	= delay_t(0);
	delay_total	= delay_t(0);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         is_null/nullify
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
	
/** \brief return true if the timer is considered null, false otherwise
 */
bool	timer_probing_t::is_null()	const throw()
{
	if( delay_current.is_null() )	return true;
	return false;
}

/** \brief nullify the object
 */
void	timer_probing_t::nullify()	throw()
{
	delay_current	= delay_t(0);
	delay_total	= delay_t(0);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         utility function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the policy to the first timer and return its value
 */
delay_t timer_probing_t::first()		throw()
{
	delay_current	= delay_begin;
	delay_total	= delay_begin;
	return delay_current;
}

/** \brief return the value of the current timer
 */
delay_t timer_probing_t::current()		throw()
{
	return delay_current;
}

/** \brief goto the next value of timer and return it
 */
delay_t	timer_probing_t::next()			throw()
{
	// if the timer is finished, return NEVER
	if( is_finished() )	return delay_t::NEVER;
	
	// update the delay_current
	delay_current	 = probe_period;
	// update the total_time
	delay_total	+= probe_period;
	// if the timer is now finished, return delay_t::NEVER
	if( is_finished() )	delay_current= delay_t::NEVER;
	// return delay_current
	return delay_current;
}

/** \brief return true if the timer is finished, false otherwise
 */
bool	timer_probing_t::is_finished()	const throw()
{
	if( delay_total > delay_end )	return true;
	return false;
}

NEOIP_NAMESPACE_END




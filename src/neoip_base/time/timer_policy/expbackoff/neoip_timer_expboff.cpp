/*! \file
    \brief Implementation of the timer_expboff_t
    
*/

/* system include */

/* local include */
#include "neoip_timer_expboff.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief the constructor
 */
timer_expboff_t::timer_expboff_t( const delay_t &delay_min
				, const delay_t &delay_max )
				: delay_min(delay_min), delay_max(delay_max), delay_current(delay_min)
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         utility function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the policy to the first timer and return its value
 */
delay_t timer_expboff_t::first()	throw()
{
	delay_current = delay_min;
	return delay_current;	
}

/** \brief return the value of the current timer
 */
delay_t timer_expboff_t::current()	throw()
{
	return delay_current;
}

/** \brief goto the next value of timer and return it
 */
delay_t	timer_expboff_t::next()	throw()
{
	// multiply the delay by 2
	delay_current = delay_current + delay_current;
	// if the current delay is greather than delay_max, clamp it
	if( delay_current > delay_max )		delay_current = delay_max;
	return delay_current;
}


/** \brief return true if the timer is finished, false otherwise
 */
bool	timer_expboff_t::is_finished()		const throw()
{
	return false;
}

NEOIP_NAMESPACE_END




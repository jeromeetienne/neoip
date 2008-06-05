/*! \file
    \brief Implementation of the delaygen_expboff_t
*/

/* system include */
/* local include */
#include "neoip_delaygen_expboff.hpp"
#include "neoip_rand.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
delaygen_expboff_t::delaygen_expboff_t(const delaygen_expboff_arg_t &arg)	throw()
{
	// copy the arg_t parameter
	DBG_ASSERT( arg.is_valid() );
	// copy the arguement
	this->arg	= arg;
	// zero some field
	inc_counter	= 0;
	elapsed_delay	= delay_t::from_sec(0);
	current_theo	= arg.first_delay();
	// handle the special case of current_theo being delay_t::is_special()
	if( current_theo.is_special() ){
		current_real	= current_theo;
	}else{
		// put some randomness 
		current_real	= current_theo * neoip_rand(1.0 - arg.random_range(), 1.0 + arg.random_range());
		// if the current_real would result in a elapsed_delay > timeout_delay, clamp it
		if( !arg.timeout_delay().is_special() && elapsed_delay+current_real > arg.timeout_delay())
			current_real = arg.timeout_delay() - elapsed_delay;
	}
}

/** \brief return the amount of time already notified as expired
 */
void	delaygen_expboff_t::increment()		throw()
{
	// sanity check - this delaygen MUST NOT be is_timedout
	DBG_ASSERT( !is_timedout() );

	// update the inc_counter
	inc_counter++;
	
	// compute the next current value
	if( inc_counter == 1 ){
		current_theo	= arg.multiplicator() >= 1.0 ? arg.min_delay() : arg.max_delay();
	}else{
		current_theo	= current_theo * arg.multiplicator();
	}
	// if the current_theo is greater than max_val, clamp it
	if( current_theo > arg.max_delay() )	current_theo	= arg.max_delay();
	// if the current_theo is less than min_val, clamp it
	if( current_theo < arg.min_delay() )	current_theo	= arg.min_delay();

	// handle the special case of current_theo being delay_t::is_special()
	if( current_theo.is_special() ){
		current_real	= current_theo;
	}else{
		// put some randomness for the real value - post clamping
		current_real	= current_theo * neoip_rand(1.0 - arg.random_range(), 1.0 + arg.random_range());
		// if the current_real would result in a elapsed_delay > timeout_delay, clamp it
		if( !arg.timeout_delay().is_special() && elapsed_delay+current_real > arg.timeout_delay())
			current_real = arg.timeout_delay() - elapsed_delay;
	}
}

/** \brief return the value of the current timer
 */
delay_t	delaygen_expboff_t::get_current_delay()	const throw()
{
	// sanity check - this delaygen MUST NOT be is_timedout
	DBG_ASSERT( !is_timedout() );
	// return the current value
	return current_real;
}

/** \brief Notify the expiration of the current delay_t
 */
void	delaygen_expboff_t::notify_expiration()	throw()
{
	// update the elapsed_delay
	elapsed_delay	+= current_real;
}


/** \brief return true if the delaygen_vapi_t is timedout, false otherwise
 */
bool	delaygen_expboff_t::is_timedout()	const throw()
{
	// if the elapsed_delay is >= to the timeout_delay, the delaygen is considered is_timedout
	if( !arg.timeout_delay().is_special() && elapsed_delay >= arg.timeout_delay() )	return true;
	// else it is considered NOT is_timedout
	return false;
}


NEOIP_NAMESPACE_END




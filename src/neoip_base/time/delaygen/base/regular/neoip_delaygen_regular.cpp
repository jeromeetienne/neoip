/*! \file
    \brief Implementation of the delaygen_regular_t
*/

/* system include */
/* local include */
#include "neoip_delaygen_regular.hpp"
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
delaygen_regular_t::delaygen_regular_t(const delaygen_regular_arg_t &arg)	throw()
{
	// copy the arg_t parameter
	DBG_ASSERT( arg.is_valid() );
	// copy the arguement
	this->arg	= arg;
	// zero some field
	current_real	= arg.first_delay();
	elapsed_delay	= delay_t::from_sec(0);	

	// handle the special case of current_real being delay_t::is_special()
	if( current_real.is_special() )	return;
	
	// put some randomness 
	current_real	= current_real * neoip_rand(1 - arg.random_range(), 1 + arg.random_range());
	// if the current_real would result in a elapsed_delay > timeout_delay, clamp it
	if( elapsed_delay + current_real > arg.timeout_delay() )
		current_real = arg.timeout_delay() - elapsed_delay;
}

/** \brief return the amount of time already notified as expired
 */
void	delaygen_regular_t::increment()		throw()
{
	// sanity check - this delaygen MUST NOT be is_timedout
	DBG_ASSERT( !is_timedout() );
	// compute the next current value
	current_real	= arg.period();
	
	// handle the special case of current_real being delay_t::is_special()
	if( current_real.is_special() )	return;
	
	// put some randomness 
	current_real	= current_real * neoip_rand(1 - arg.random_range(), 1 + arg.random_range());
	// if the current_real would result in a elapsed_delay > timeout_delay, clamp it
	if( elapsed_delay + current_real > arg.timeout_delay() )
		current_real = arg.timeout_delay() - elapsed_delay;
}

/** \brief return the value of the current timer
 */
delay_t	delaygen_regular_t::get_current_delay()	const throw()
{
	// sanity check - this delaygen MUST NOT be is_timedout
	DBG_ASSERT( !is_timedout() );
	// return the current value
	return current_real;
}

/** \brief Notify the expiration of the current delay_t
 */
void	delaygen_regular_t::notify_expiration()	throw()
{
	// update the elapsed_delay
	elapsed_delay	+= current_real;
}


/** \brief return true if the delaygen_vapi_t is timedout, false otherwise
 */
bool	delaygen_regular_t::is_timedout()	const throw()
{
	// if the elapsed_delay is >= to the timeout_delay, the delaygen is considered is_timedout
	if( elapsed_delay >= arg.timeout_delay() )	return true;
	// else it is considered NOT is_timedout
	return false;
}


NEOIP_NAMESPACE_END




/*! \file
    \brief Header of the timer_policy
    
*/


#ifndef __NEOIP_TIMER_POLICY_HPP__ 
#define __NEOIP_TIMER_POLICY_HPP__ 
/* system include */

/* local include */
#include "neoip_delay.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN
/** \brief the virtual API for all timer_policy_t
 */
class timer_policy_t {
public:
	//! reset the timer policy
	void	reset() { first(); }
	//! set the policy to the first timer and return its value
	virtual	delay_t	first() 		throw() = 0;
	//! return the value of the current timer
	virtual	delay_t	current()		throw()	= 0;
	//! goto the next value of timer and return it
	virtual	delay_t	next()			throw()	= 0;
	//! return true if the timer is finished, false otherwise
	virtual bool	is_finished()		const throw() = 0;
	//! virtual destructor
	virtual ~timer_policy_t() {};
};
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TIMER_POLICY_HPP__  */







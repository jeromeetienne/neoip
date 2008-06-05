/*! \file
    \brief Header of the timer_expboff
    
*/


#ifndef __NEOIP_TIMER_EXPBACKOFF_HPP__ 
#define __NEOIP_TIMER_EXPBACKOFF_HPP__ 
/* system include */

/* local include */
#include "neoip_timer_policy.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief the virtual API for all timer_expboff_t
 */
class timer_expboff_t : public timer_policy_t {
private:
	delay_t	delay_min;
	delay_t	delay_max;
	delay_t	delay_current;
public:
	timer_expboff_t( const delay_t &delay_min = delay_t::from_sec(1)
					, const delay_t &delay_max = delay_t::from_sec(30));
					
	delay_t		first()			throw();
	delay_t		current()		throw();
	delay_t		next()			throw();
	bool		is_finished()		const throw();
	
};

NEOIP_NAMESPACE_END
#endif	/* __NEOIP_TIMER_EXPBACKOFF_HPP__  */







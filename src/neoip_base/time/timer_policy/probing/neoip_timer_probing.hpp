/*! \file
    \brief Header of the timer_probing

- TODO likely a good idea to remove the max_probe and to give a probe_period

*/


#ifndef __NEOIP_TIMER_PROBING_HPP__ 
#define __NEOIP_TIMER_PROBING_HPP__ 
/* system include */

/* local include */
#include "neoip_timer_policy.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief the virtual API for all timer_probing_t
 */
class timer_probing_t : public timer_policy_t {
private:
	delay_t		delay_begin;
	delay_t		delay_end;
	delay_t		probe_period;	//!< the delay between probe
	
	delay_t		delay_current;
	delay_t		delay_total;
public:
	timer_probing_t()	throw()	{ nullify(); }
	timer_probing_t(const delay_t &delay_begin, const delay_t &delay_end, const delay_t &probe_period)
										throw();
	
	bool			is_null()	const throw();
	void			nullify()	throw();
	
	delay_t			first()		throw();
	delay_t			current()	throw();
	delay_t			next()		throw();
	bool			is_finished()	const throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TIMER_PROBING_HPP__  */







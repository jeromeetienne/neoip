/*! \file
    \brief Header of the \ref rat0_limit_t

*/


#ifndef __NEOIP_RAT0_LIMIT_HPP__ 
#define __NEOIP_RAT0_LIMIT_HPP__ 
/* system include */
/* local include */
#include "neoip_rat0_estim.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \ref class to limit the rate of some occurences (e.g. byte sent/recv over a connection)
 * 
 * - TODO the technic used here suffers from a ping-pong effect 
 *   - launch the utest and stop it with ctrl-s/q to see the ping-pong
 */
template <typename T_ELEM> class rat0_limit_t : public rat0_estim_t<T_ELEM> {
private:
public:
	//! Constructor
	rat0_limit_t()	throw() {}
	rat0_limit_t(const delay_t &total_delay, const delay_t &granularity)	throw()
			: rat0_estim_t<T_ELEM>(total_delay, granularity)	{}

	/** \brief return true if the limit is exceeded, false otherwise
	 * 
	 * @param occurence_per_sec the limit of occurence per second
	 */
	bool	is_exceeded(T_ELEM occurence_per_sec)	throw()	{
		double	average = rat0_estim_t<T_ELEM>::get_avg(delay_t::from_sec(1));
		if( average > occurence_per_sec	)	return true;
		return false;
	}
	/** \brief  return true if to add this occurence would exceed the limit, false otherwise
	 * 
	 * @param occurence_would_add the occurence that IF it is added would cause or 
	 *                            NOT the limit
	 * @param occurence_per_sec the limit of occurence per second
	 */
	bool	would_exceed(T_ELEM occurence_would_add, T_ELEM occurence_per_sec)	throw()	{
		delay_t	total_delay	= rat0_estim_t<T_ELEM>::get_total_delay();
		double	average_total	= rat0_estim_t<T_ELEM>::get_avg(delay_t::from_sec(1), total_delay
									, occurence_would_add);
		if( average_total > occurence_per_sec	)	return true;
		return false;
	}	
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_RAT0_LIMIT_HPP__  */











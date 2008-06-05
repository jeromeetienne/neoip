/*! \file
    \brief Header of the \ref rat0_estim_t

- This layer implement a rate estimator, independantly of the kind
  of rate measured.
  - it can be a rate in disk writing in byte, in network reading in packet, anything
- it can be used in conjuction with rat0_limit.c to obtain a rate limiter

\par Algorithm Description
- it maintains a array of counter per jiffies.
  - a jiffie is the minimal amount of time per counter
    - the jiffies is the jiffie_period/precision of the estimation
    - it is expressed in millisecond
    - at each new rat0_estim_add(), the new value is added to the counter of 
      the jiffies representing the present time.
- Based on this array, the average is computed over a number of jiffies provided
  by the caller in rat0_estim_get_avg()

\par Implementation Notes
- the array of counter is stored in a sliding window (see \ref slidwin_t), so its size
  doesn't affect the cpu usage for insert 
  - TODO but it does for the average which recount all the slidwin_t every time
    it could be avoided by having an incrementale total using callback in slidwin_t	

*/


#ifndef __NEOIP_RAT0_ESTIM_HPP__ 
#define __NEOIP_RAT0_ESTIM_HPP__ 
/* system include */
/* local include */
#include "neoip_date.hpp"
#include "neoip_slidwin.hpp"
#include "neoip_log.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \ref class to store the packet type sent by \ref ns-peer
 */
template <typename T_ELEM> class rat0_estim_t : NEOIP_COPY_CTOR_ALLOW {
private:
	slidwin_t<T_ELEM, uint64_t>		slidwin;

	int	nb_jiffie;	//!< the number of jiffie over which this estimation is done
	delay_t	jiffie_period;	//!< the amount of time for a single jiffie
	
	

	/** \brief return the average over the counted elements of the rat0_estim_t
	 * 
	 * @param range_jiffie_out if a range_jiffie_out is provided, return the range 
	 * 	 	  	    of the estimation in jiffie
	 * @return the total of all occurences during the estamation coverage
	 */
	T_ELEM	do_total(delay_t start_jiffie, delay_t *range_jiffie_out = NULL)	throw() {
		// sanity check - slidwin MUST already be created
		DBG_ASSERT( slidwin.size() );

		delay_t	present_jiffie	= date_t::present() / jiffie_period;
		delay_t	end_jiffie	= delay_t(slidwin.get_index_last());
		T_ELEM	total		= 0;
		// if the sliding window covers future time, dont count it
		// NOTE: this can happen at the begining of the estimation when the age
		//       of the estimation is less than the slidwin time span
		if( end_jiffie > present_jiffie )	end_jiffie	= present_jiffie;
		
		// if the slidwin window covers time before start_jiffie, dont count them
		if( start_jiffie < delay_t(slidwin.get_index_first()) )
			start_jiffie = delay_t(slidwin.get_index_first());

		// some logging to debug
		KLOG_DBG("compute total from " << date_t(jiffie_period*start_jiffie) 
					<< " to " << date_t(jiffie_period*end_jiffie));
		// compute the average over the slidwin
		for(delay_t jiffie = start_jiffie; jiffie <= end_jiffie; jiffie++)
			total	+= slidwin.get(jiffie.to_uint64());
		
		// if a range_jiffie_out is provided, return the range of the estimation in jiffie
		if( range_jiffie_out )	*range_jiffie_out = end_jiffie - start_jiffie + delay_t(1);

		// return the result
		return total;
	}

	/** \brief create the sliding window
	 * 
	 * - the slidwin_t creation is delayed to start the slidwin_t at the first usage()
	 *   and not at the rat0_estim_t creation
	 */  
	void create_slidwin_if_needed()	throw(){
		// if the slidwin is already created, do nothing
		if( slidwin.size() )	return;
		// compute the present in jiffie
		delay_t	present_jiffie	= date_t::present() / jiffie_period;
		// create the slidwin
		slidwin = slidwin_t<T_ELEM, uint64_t>(nb_jiffie, 0, present_jiffie.to_uint64());
	}

public:
	//! Constructor
	rat0_estim_t()	throw() : nb_jiffie(0), jiffie_period(delay_t::from_sec(0))	{}
	rat0_estim_t(const delay_t &total_delay, const delay_t &jiffie_period)	throw(){
		// some logging to debug
		KLOG_DBG("total delay=" << total_delay);
		KLOG_DBG("jiffie_period=" << jiffie_period);
		// sanity check - the total delay MUST be multiple of jiffie_period
		DBG_ASSERT( total_delay % jiffie_period == delay_t(0) );
		// compute the number of jiffie
		nb_jiffie		= delay_t(total_delay / jiffie_period).to_uint32();
		// copy the jiffie period
		this->jiffie_period	= jiffie_period;
	}
	~rat0_estim_t()	throw() {
	}
	
	delay_t	get_total_delay()	const throw()	{ return jiffie_period * nb_jiffie;	}
	delay_t	get_jiffie_period()	const throw()	{ return jiffie_period;			}
	
	//! count new occurence of the measured event
	void	count(T_ELEM nb_occurence)	throw()
	{
		// create slidwin if needed
		create_slidwin_if_needed();
		// compute the present in jiffie
		delay_t	present_jiffie	= date_t::present() / jiffie_period;
		// if needed, add the previous occurence for the same jiffie		
		if( slidwin.exist(present_jiffie.to_uint64()) )
			nb_occurence += slidwin.get(present_jiffie.to_uint64());
		// set the new number of occurence for this jiffie
		slidwin.set( present_jiffie.to_uint64(), nb_occurence );
	}
	
	
	/** \brief return the estimated average estimated over <range_delay>
	 *         computed over a <unit>
	 * 
	 * - e.g. get_avg(delay_t::from_sec(10), delay_t::from_sec(2)) return
	 *   the average estimated over the last 10sec computed for 2sec
	 *   - if 1-mbyte has been transfered in 10sec, the result will be 200kbyte
	 */
	double	get_avg(const delay_t &unit = delay_t::from_sec(1), delay_t range_delay = delay_t(), T_ELEM total_offset = 0) throw() {
		// create slidwin if needed
		create_slidwin_if_needed();
		// slide the window to the current jiffie
		count(0);	
		// if range_delay is not set, set it to the window range
		if( range_delay.is_null() )	range_delay = jiffie_period * delay_t(nb_jiffie);
		// compute the jiffie on which start the estimation
		delay_t	start_jiffie	= (date_t::present() - range_delay) / jiffie_period;
		// compute the total of all occurence for the estimation
		delay_t	range_jiffie;
		T_ELEM	total		= do_total(start_jiffie, &range_jiffie);
		// add the total_offset
		total	+= total_offset;
		// some logging to debug
		KLOG_DBG("avg computed over " << range_jiffie * jiffie_period << " total="<< total
						<< " unit=" << unit );
		// compute the delay over which the average will be computed
		delay_t avg_delay	= range_jiffie * jiffie_period;
		if( avg_delay < unit )	avg_delay = unit;
		// convert the total to be expressed per unit
		return (double)total * unit.to_double() / avg_delay.to_double();
	}
	
	/** \brief return the total of all occurend over <range_delay>
	 */
	T_ELEM	get_total(delay_t range_delay = delay_t())	throw()	{
		// create slidwin if needed
		create_slidwin_if_needed();
		// slide the window to the current jiffie
		count(0);	
		// if range_delay is not set, set it to the window range
		if( range_delay.is_null() )	range_delay = jiffie_period * delay_t(nb_jiffie);
		// compute the jiffie on which start the estimation
		delay_t	start_jiffie = (date_t::present() - range_delay) / jiffie_period;
		// compute the total of all occurence for the estimation
		return do_total(start_jiffie);
	}
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_RAT0_ESTIM_HPP__  */











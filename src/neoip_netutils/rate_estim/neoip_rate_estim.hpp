/*! \file
    \brief Header of the \ref rate_estim_t

- This layer implement a rate estimator, independantly of the kind of rate measured.
  - it can be a rate in disk writing in byte, in network reading in packet, anything

\par Algorithm Description
- it maintains a array of event counter per slice.
- a slice represent the counter of event for a given period of time, long of slice_delay
- Based on this array, the average is computed over the number of valid slice.
- the cur_size indicate the number of valid slice, this is done to get a good estimation
  even if the estimation just started
  - without it, an average over the whole slice_arr, with most value being 0 as unset
    will produce an underestimated average.

\par About mutable and const in this object
- the average() function is to read the current estimation of the rate. so intuitively
  it should be a const function.
- in practice, it may slide the slice_arr to the present so modify a lot of fields
- consequently of lot of the fields are marked 'mutable'
- better put the trick here that doing it on the caller layer (as i did in a previous
  version)
  
\par Possible Improvement
- DONE - in ::average(), the old version have been left in case a mistake is in the new code
  currently a slice is always used entirely, this mean that is the slide_delay 
  is 2sec, every 2 sec the rate estimation will be underevaluated
  - it could be avoided by sliding the estimation according to the current_time
  - e.g. if the present time is 0.5sec within a slice, the oldest slice is 
    multiplied by (2-0.5)/2 and the newest slice is multiplied by 0.5/2
  - this would provide a much smoother evaluation and avoid the underevaluation
    on slice edge.

*/


#ifndef __NEOIP_RATE_ESTIM_HPP__ 
#define __NEOIP_RATE_ESTIM_HPP__ 
/* system include */
#include <math.h>
/* local include */
#include "neoip_rate_estim_arg.hpp"
#include "neoip_date.hpp"
#include "neoip_math_shortcut.hpp"
#include "neoip_log.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief to estimate a rate of event
 */
template <typename T> class rate_estim_t : NEOIP_COPY_CTOR_ALLOW {
private:
	mutable std::vector<T>	slice_arr;//!< the array containing the slice counting the events
	delay_t		slice_delay;	//!< the delay represented by a single slice
	mutable size_t	cur_size;	//!< the current number of 'valid' slice (MUST BE >= 1)
	mutable size_t	beg_offs;	//!< the offset of the first slice within the slice_arr		
	mutable date_t	beg_date;	//!< the date_t of the first slice
	mutable T	precpu_sum;	//!< the sum of all the 'valid' slice (precomputed for lower cpu)
	
	/*************** Internal function	*******************************/
	void		internal_ctor(const delay_t &total_delay, const delay_t &slice_delay)	throw();
	size_t		tot_size()		const throw()	{ return slice_arr.size();	}
	size_t		end_offs()		const throw()	{ return (beg_offs+cur_size-1)%tot_size();}
	void		reset_arr()		const throw();
	void		slide_to_date(const date_t &date)	const throw();
	void		advance_arr(size_t nb_slice)		const throw();
public:
	/*************** ctor/dtor	***************************************/
	rate_estim_t()								throw() {}
	rate_estim_t(const delay_t &total_delay, const delay_t &slice_delay)	throw()	{ internal_ctor(total_delay, slice_delay);		}
	rate_estim_t(const rate_estim_arg_t &arg)				throw()	{ internal_ctor(arg.total_delay(), arg.slice_delay());	}

	/*************** query function	***************************************/
	bool		is_null()		const throw()	{ return slice_arr.empty();	}
	const T &	total()			const throw()	{ return precpu_sum;		}
	double		average()		const throw();
	
	/*************** Action function	*******************************/
	void		update(T nb_event)	throw();
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//		ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor which produce a gen_id_t
 * 
 * - the number of slices will be total_delay/slice_delay
 * - for cleaness, total_delay MUST be dividable by slice_delay
 */
template <typename T>
void rate_estim_t<T>::internal_ctor(const delay_t &total_delay, const delay_t &slice_delay)	throw()
{
	// sanity check - the total delay MUST be multiple of slice_delay
	DBG_ASSERT( total_delay % slice_delay == delay_t(0) );
	// copy the slice_delay
	this->slice_delay	= slice_delay;
	// compute the number of slice
	size_t	nb_slice	= delay_t(total_delay / slice_delay).to_uint32();
	// allocate the slice_arr
	slice_arr.assign(nb_slice, 0);
	// reset the rate_estim_t to set the variable values
	reset_arr();
}

/** \brief Update the rate estimation with a number of event
 */
template <typename T>
void	rate_estim_t<T>::update(T nb_events)					throw()
{
	// slide the slice_arr to the present if needed
	slide_to_date(date_t::present());
	
	// update the last slice (the one corresponding to the present)
	slice_arr[end_offs()]	+= nb_events;
	// update the precomputed sum
	precpu_sum		+= nb_events;
}

/** \brief Return the estimated average of this rate per second
 * 
 * - this is a const as it is intuitivly only a query function
 *   - but in practice it may slide the array if needed, so it does
 *     some funkyness with mutable.
 * - TODO to rename rate_average()
 */
template <typename T>
double	rate_estim_t<T>::average()		const throw()
{
	date_t	present	= date_t::present();
	// log to debug
	KLOG_DBG("enter cur_size=" << cur_size);
	// slide the slice_arr to the present if needed
	slide_to_date(present);

	// compute the delta between now and the oldest counted slice
	delay_t	delay_delta	= present - beg_date;
	
	// estimate a minimum of 1 slice
	// - NOTE: this avoid a overestimation when estimation just started
	//   - happen because the rate update is discret
	delay_delta	= std::max(delay_delta, slice_delay);
	
	// return the average of the sum from all the counted slices by the delay_delta
	return double(precpu_sum) / delay_delta.to_sec_double();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//		Internal function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Reset the rate_estim_t aka blank the whole estimation
 */
template <typename T>
void	rate_estim_t<T>::reset_arr()						const throw()
{
	// log to debug
	KLOG_DBG("enter");
	// set the variable to have a single slice
	cur_size	= 1;
	beg_offs	= 0;
	beg_date	= date_t::present();
	// zero the precpu_sum
	precpu_sum	= 0;
	// zero the slice
	slice_arr[beg_offs]	= 0;
}

/** \brief advance the slice_arr of nb_slice
 */
template <typename T>
void	rate_estim_t<T>::advance_arr(size_t nb_slice)				const throw()
{
	// log to debug
	KLOG_DBG("enter nb_slice=" << nb_slice);
	// sanity check - the nb_slice MUST be less than tot_size() (if >=, one should use reset_arr())
	DBG_ASSERT( nb_slice < tot_size() );
	// advance of nb_slice
	for(size_t i = 0; i < nb_slice; i++){
		// if cur_size == tot_size, a slice MUST be removed before initializing a new one
		if( cur_size == tot_size() ){
			// update the precpu_sum
			DBG_ASSERT( precpu_sum >= slice_arr[beg_offs] );
			precpu_sum	-= slice_arr[beg_offs];
			// increase the beg_offs and beg_date
			beg_offs	= (beg_offs + 1) % tot_size();
			beg_date	+= slice_delay;
			// reduce the cur_size
			DBG_ASSERT( cur_size >= 2 );
			cur_size--;
		}
		// sanity check - the room for a new slice MUST be available
		DBG_ASSERT( cur_size < tot_size() );
		// update the cur_size - to create a new slice
		cur_size++;
		// zero the value of this new slice
		slice_arr[end_offs()]	= 0;
	}
}
/** \brief Slide the slice_arr to the present if needed
 */
template <typename T>
void	rate_estim_t<T>::slide_to_date(const date_t &date)			const throw()
{
	// compute the delta compare to the first valid slice
	delay_t	date_delta	= date - beg_date;
	size_t	offs_delta	= delay_t(date_delta / slice_delay).to_uint32();
	// log to debug
	KLOG_DBG("enter date_delta=" << date_delta << " offs_delta=" << offs_delta);
	// sanity check - as the 'present' is always increasing, offs_delta MUST be the last slice or more
	DBG_ASSERT( offs_delta >= cur_size-1 );
	// advance the slice_arr
	size_t	nb_step	= offs_delta - (cur_size-1);
	// if the nb_step to reach the present if larger than the slice_arr, reset it completly
	if( nb_step >= tot_size() ){
		reset_arr();
		return;
	}
	// advance the slice_arr
	advance_arr( nb_step );
}

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_RATE_ESTIM_HPP__  */











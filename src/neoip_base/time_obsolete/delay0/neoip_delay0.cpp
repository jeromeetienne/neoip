/*! \file
    \brief Implementation of the \ref delay0_t
    
\par Possible Improvement
- the is_special() function has a poor name. this 'special' stuff is bad name
  - sure but currently i dont find any better one
*/

/* system include */
#include <time.h>
#include <assert.h>
#include <iostream>

/* local include */
#include "neoip_delay0.hpp"
#include "neoip_log.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                               CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	delay0_t::to_string()	const throw()
{
	std::ostringstream	oss;

	// handle the special values
	if( is_null() )		return "null";
	else if( is_never() )	return "never";
	else if( is_always() )	return "always";


	// TODO: wow this is a the cleanest code :) EVER!!
	if( val_ms == 0 )	return "0s";

#define DELAY_T_CPUBASE(base_val, base_str)				\
	if( nb_ms >= uint64_t(base_val) ){				\
		uint64_t	val	= nb_ms / uint64_t(base_val);	\
		nb_ms	-= val * uint64_t(base_val);			\
		oss << val << base_str;				\
	}	

	// display the delay
	uint64_t	nb_ms = val_ms;
	DELAY_T_CPUBASE( 30ULL * 24ULL * 60ULL * 60ULL * 1000ULL, "month" );
	DELAY_T_CPUBASE(         24ULL * 60ULL * 60ULL * 1000ULL, "d" );
	DELAY_T_CPUBASE(                 60ULL * 60ULL * 1000ULL, "h" );
	DELAY_T_CPUBASE(                         60ULL * 1000ULL, "m" );
	DELAY_T_CPUBASE(                                 1000ULL, "s" );
	DELAY_T_CPUBASE(                                    1ULL, "ms" );
#undef DELAY_T_CPUBASE
	return oss.str();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          operation + and -
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the + operator
 */
delay0_t delay0_t::operator+( const delay0_t & other ) const
{
	// sanity check	- if the local or other delay0_t is "special", the operation is undefined
	// - as i dont know what to do with it, i do assert for now
	DBG_ASSERT( !is_special() );
	DBG_ASSERT( !other.is_special() );
	// do the operation
	return this->val_ms + other.val_ms;
}

/** \brief overload the - operator
 */
delay0_t delay0_t::operator-( const delay0_t & other ) const
{
	// sanity check	- if the local or other delay0_t is "special", the operation is undefined
	// - as i dont know what to do with it, i do assert for now
	DBG_ASSERT( !is_special() );
	DBG_ASSERT( !other.is_special() );
	// do the operation
	return this->val_ms - other.val_ms;
}

/** \brief overload the / operator
 */
delay0_t delay0_t::operator / ( const delay0_t & other ) const
{
	// sanity check	- if the local or other delay0_t is "special", the operation is undefined
	// - as i dont know what to do with it, i do assert for now
	DBG_ASSERT( !is_special() );
	DBG_ASSERT( !other.is_special() );
	// do the operation
	return this->val_ms / other.val_ms;
}

/** \brief overload the * operator
 */
delay0_t delay0_t::operator * ( const delay0_t & other ) const
{
	// sanity check	- if the local or other delay0_t is "special", the operation is undefined
	// - as i dont know what to do with it, i do assert for now
	DBG_ASSERT( !is_special() );
	DBG_ASSERT( !other.is_special() );
	// do the operation
	return this->val_ms * other.val_ms;
}


/** \brief overload the * operator
 */
delay0_t delay0_t::operator * (const double factor) const
{
	// if the delay0_t is_special, return it unchanged
	if( is_special() )	return *this;
	// do the operation
	return uint64_t(this->val_ms * factor);
}


/** \brief overload the % operator
 */
delay0_t delay0_t::operator % ( const delay0_t & other ) const
{
	// sanity check	- if the local or other delay0_t is "special", the operation is undefined
	// - as i dont know what to do with it, i do assert for now
	DBG_ASSERT( !is_special() );
	DBG_ASSERT( !other.is_special() );
	// do the operation
	return this->val_ms % other.val_ms;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           main compare function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int	delay0_t::compare(const delay0_t &other)	const throw()
{
	// logging to debug
	KLOG_DBG("compare this=" << *this << " with other=" << other );
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// if the local object is_special()
	if( is_special() ){
		// if the local object is_never()
		if( is_never() && other.is_always() )	return -1;
		if( is_never() && other.is_never() )	return  0;
		if( is_never() && !other.is_special())	return -1;
		// if the local object is_always()
		if( is_always() && !other.is_special())	return +1;
		if( is_always() && other.is_always() )	return  0;
		if( is_always() && other.is_never() )	return +1;
		// NOTE: this point MUST NEVER be reached
		DBG_ASSERT( 0 );
	}
	// NOTE: here the local object is NOT special

	// if the other object is_special()
	// - simplified as the local object is NOT special
	if( other.is_special() ){
		if( other.is_never() )	return +1;
		if( other.is_always() )	return -1;
		// NOTE: this point MUST NEVER be reached
		DBG_ASSERT( 0 );
	}
	// NOTE: here the local and the other object are NOT special

	// compare the value itself
	if( val_ms < other.val_ms )		return -1;
	if( val_ms > other.val_ms )		return +1;

	// here both object are equal
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           main compare function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a bit less that the base delay0_t
 * 
 * - This function is a little bit 'funky'.
 * - its primary intended use is to return a delay0_t which is a little bit before
 *   another one. typically 2 timeout and willing to ensure one is triggered before
 *   the other.
 * 
 * @param delta_max	the maximum delta between the base time and the returned value (default 1sec)
 * @param delta_ratio	the delta_ratio to use
 * @return the maximum of (this * delta_ratio, this - delta_max)
 */
delay0_t	delay0_t::a_bit_less(const delay0_t &delta_max, double delta_ratio)	const throw()
{
	// if this delay is special, return it unchanged
	if( this->is_special() )	return *this;
	// compute this delta after applying the delta_ratio
	delay0_t	tmp	= *this * delta_ratio;
	// if this * delta_ratio > this - delta_max, return this * delta_ratio
	// - NOTE: test rewritten to avoid to substract delta_max as delay0_t can't be negative
	if( tmp + delta_max > *this )	return tmp;
	// return this - delta_max
	return *this - delta_max;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial delay0_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a delay0_t
 */
serial_t &operator << (serial_t& serial, const delay0_t &delay)		throw()
{
	serial	<< delay.val_ms;
	return serial;
}

/** \brief unserialize a delay0_t
 */
serial_t &operator >> (serial_t & serial, delay0_t &delay)		throw(serial_except_t)
{
	serial >> delay.val_ms;
	return serial;
}


NEOIP_NAMESPACE_END




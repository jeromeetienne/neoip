/*! \file
    \brief Implementation of the \ref delay2_t
    
\par Possible Improvement
- the is_special() function has a poor name. this 'special' stuff is bad name
  - sure but currently i dont find any better one
  
- remove the implicit ctor for uint64_t
- remove the ALWAYS/NEVER to get only INFINITE
  - replace is_special() by is_infinite
- keep a compatibility layer for ALWAYS/NEVER
*/

/* system include */
#include <time.h>
#include <iostream>
/* local include */
#include "neoip_delay2.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref delay2_t constant
const delay2_t	delay2_t::INFINITE	= delay2_t(0xFFFFFFFFFFFFFFEULL);
const delay2_t	delay2_t::NONE		= delay2_t(0xFFFFFFFFFFFFFFFULL);
const delay2_t	delay2_t::ALWAYS	= delay2_t(0xFFFFFFFFFFFFFFEULL);
const delay2_t	delay2_t::NEVER		= delay2_t(0xFFFFFFFFFFFFFFEULL);
// end of constants definition


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a bit less that the base delay2_t
 * 
 * - This function is a little bit 'funky'.
 * - its primary intended use is to return a delay2_t which is a little bit before
 *   another one. typically 2 timeout and willing to ensure one is triggered before
 *   the other.
 * 
 * @param delta_max	the maximum delta between the base time and the returned value (default 1sec)
 * @param delta_ratio	the delta_ratio to use
 * @return the maximum of (this * delta_ratio, this - delta_max)
 */
delay2_t	delay2_t::a_bit_less(const delay2_t &delta_max, double delta_ratio)	const throw()
{
	// if this delay is special, return it unchanged
	if( *this == delay2_t::INFINITE )	return *this;
	// compute this delta after applying the delta_ratio
	delay2_t	tmp	= *this * delta_ratio;
	// if this * delta_ratio > this - delta_max, return this * delta_ratio
	// - NOTE: test rewritten to avoid to substract delta_max as delay2_t can't be negative
	if( tmp + delta_max > *this )	return tmp;
	// return this - delta_max
	return *this - delta_max;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       arithmetic operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief operator +=
 */
delay2_t &	delay2_t::operator +=(const delay2_t &other)	throw()
{
	// sanity check - both operand MUST be non null
	DBG_ASSERT( !is_null() && !other.is_null() );
	// if any other 2 operands is infinite, set the result to infinite
	if( is_infinite() || other.is_infinite() )	return *this = INFINITE;
	// do the operation itself
	val_ms += other.val_ms; 
	// return the object itself
	return *this;
}

/** \brief operator -=
 */
delay2_t &	delay2_t::operator -=(const delay2_t &other)	throw()
{
	// sanity check - both operand MUST be non null
	DBG_ASSERT( !is_null() && !other.is_null() );
	// if any other 2 operands is infinite, set the result to infinite
	if( is_infinite() || other.is_infinite() )	return *this = INFINITE;
	// do the operation itself
	val_ms -= other.val_ms; 
	// return the object itself
	return *this;
}

/** \brief operator *=
 */
delay2_t &	delay2_t::operator *=(const delay2_t &other)	throw()
{
	// sanity check - both operand MUST be non null
	DBG_ASSERT( !is_null() && !other.is_null() );
	// if any other 2 operands is infinite, set the result to infinite
	if( is_infinite() || other.is_infinite() )	return *this = INFINITE;
	// do the operation itself
	val_ms *= other.val_ms; 
	// return the object itself
	return *this;
}

/** \brief operator /=
 */
delay2_t &	delay2_t::operator /=(const delay2_t &other)	throw()
{
	// sanity check - both operand MUST be non null
	DBG_ASSERT( !is_null() && !other.is_null() );
	// if any other 2 operands is infinite, set the result to infinite
	if( is_infinite() || other.is_infinite() )	return *this = INFINITE;
	// do the operation itself
	val_ms /= other.val_ms; 
	// return the object itself
	return *this;
}

/** \brief operator %=
 */
delay2_t &	delay2_t::operator %=(const delay2_t &other)	throw()
{
	// sanity check - both operand MUST be non null
	DBG_ASSERT( !is_null() && !other.is_null() );
	// if any other 2 operands is infinite, set the result to infinite
	if( is_infinite() || other.is_infinite() )	return *this = INFINITE;
	// do the operation itself
	val_ms %= other.val_ms; 
	// return the object itself
	return *this;
}

/** \brief overload the * operator - special case to get percent from a delay2_t
 */
delay2_t delay2_t::operator * (const double &factor)				const throw()
{
	// sanity check - this delay2_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// if this delay2_t is infinite, dont change it
	if( is_infinite() )	return *this;
	// do the operation
	return delay2_t(uint64_t(val_ms * factor));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	delay2_t::to_string()	const throw()
{
	std::ostringstream	oss;

	// handle the special values
	if( is_null() )		return "null";
	else if(is_infinite())	return "infinite";

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
//                          serial delay2_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a delay2_t
 */
serial_t &operator << (serial_t& serial, const delay2_t &delay)		throw()
{
	// serialize the value
	serial	<< delay.val_ms;
	// return the object itself
	return serial;
}

/** \brief unserialize a delay2_t
 */
serial_t &operator >> (serial_t & serial, delay2_t &delay)		throw(serial_except_t)
{
	// unserialize the value
	serial >> delay.val_ms;
	// return the object itself
	return serial;
}


NEOIP_NAMESPACE_END




/*! \file
    \brief 
*/


#ifndef __NEOIP_RAND_HPP__ 
#define __NEOIP_RAND_HPP__ 
/* system include */
#include <stdlib.h>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief return a random double number between min_value and max_value
 */
inline double neoip_rand( double min_value, double max_value )
{
	return min_value + (max_value-min_value)*rand()/(RAND_MAX+1.0);
}

/** \brief return the value randomized by the random rate
 */
inline double neoip_rand_rate(double value, double random_rate)
{
	return neoip_rand( value - value*random_rate, value + value*random_rate);
}

/** \brief return a random double number between 0 and max_value
 * 
 * @param max_value maximum value of the random value. it has a default value of 1.0
 */
inline double neoip_rand( double max_value = 1.0 )
{
	return neoip_rand( 0.0, max_value );
}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RAND_HPP__  */




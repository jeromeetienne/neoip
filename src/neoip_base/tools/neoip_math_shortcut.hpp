/*! \file
    \brief 
*/


#ifndef __NEOIP_MATH_SHORTCUT_HPP__ 
#define __NEOIP_MATH_SHORTCUT_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief compute a division with a ceil on the result
 * 
 * - NOTE: this works only on integer.
 * - TODO make this limitation appears in the function name
 */
template <typename T> T ceil_div(const T &num, const T &div)	throw()
{
	return (num + (div-1)) / div;
}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_MATH_SHORTCUT_HPP__  */




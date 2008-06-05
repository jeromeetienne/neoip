/*! \file
    \brief Declaration of the base_oswarp_T

*/


#ifndef __NEOIP_BASE_OSWARP_HPP__ 
#define __NEOIP_BASE_OSWARP_HPP__ 
/* system include */
#include <time.h>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief helper to do file operation which depends on the operating system
 */
class	base_oswarp_t {
public:
	static int	fnmatch(const char *pattern, const char *string, int flags)	throw();
	static time_t	timegm(struct tm *tm)						throw();
	static void *	memmem(const void *haystack, size_t haystacklen
						, const void *needle, size_t needlelen)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BASE_OSWARP_HPP__  */




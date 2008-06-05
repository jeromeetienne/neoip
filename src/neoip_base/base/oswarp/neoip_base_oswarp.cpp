/*! \file
    \brief Definition of the \ref base_oswarp_t class
    
*/

/* system include */
#include <time.h>
#include <string.h>
/* local include */
#include "neoip_base_oswarp.hpp"
#include "neoip_fnmatch.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			fnmatch
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Do a fnmatch
 * 
 * - NOTE: required because WIN32 mingw doesnt provide one
 */
int	base_oswarp_t::fnmatch(const char *pattern, const char *string, int flags)	throw()
{
	return ::fnmatch(pattern, string, flags);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timegm
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef	_WIN32
	/** Yield true iff <b>y</b> is a leap-year. */
	#define IS_LEAPYEAR(y) (!(y % 4) && ((y % 100) || !(y % 400)))
	/** Helper: Return the number of leap-days between Jan 1, y1 and Jan 1, y2. */
	static int n_leapdays(int y1, int y2)
	{
		--y1;
		--y2;
		return (y2/4 - y1/4) - (y2/100 - y1/100) + (y2/400 - y1/400);
	}
	/** Number of days per month in non-leap year; used by tor_timegm. */
	static const int days_per_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#endif

/** \brief Do a timegm (which is a non standard function)
 */
time_t	base_oswarp_t::timegm(struct tm *tm)				throw()
{
#ifndef	_WIN32
	return ::timegm(tm);
#else
	// custome timegm taken from tor source which is under BSD3clause
	// - tor/src/common/utils.c
	// - just a few cosmetic change

	/* This is a pretty ironclad timegm implementation, snarfed from Python2.2.
	* It's way more brute-force than fiddling with tzset().
	*/
	time_t ret;
	unsigned long year, days, hours, minutes;
	int i;
	year = tm->tm_year + 1900;
	if(year < 1970 || tm->tm_mon < 0 || tm->tm_mon > 11){
		KLOG_ERR("Out-of-range argument to base_oswarp_t::timegm");
		return -1;
	}
	days = 365 * (year-1970) + n_leapdays(1970,year);
	for(i = 0; i < tm->tm_mon; ++i)
		days += days_per_month[i];
	if(tm->tm_mon > 1 && IS_LEAPYEAR(year))	days	+= 1;
	days	+= tm->tm_mday - 1;
	hours	= days*24 + tm->tm_hour;
	minutes	= hours*60 + tm->tm_min;
	ret	= minutes*60 + tm->tm_sec;
	return ret;
#endif
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			memmem
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Do a memmem
 * 
 * - NOTE: memmem is not present in mingw
 */
void *	base_oswarp_t::memmem(const void *haystack, size_t haystacklen
					, const void *needle, size_t needlelen)	throw()
{
#ifndef	_WIN32
	return	::memmem(haystack, haystacklen, needle, needlelen);
#else
	const char *begin;
	const char *const last_possible = (const char *) haystack + haystacklen - needlelen;

	// The first occurrence of the empty string is deemed to occur at the beginning of the string.  */
	if( needlelen == 0 )		return (void *) haystack;

	// Sanity check, otherwise the loop might search through the whole memory.
	if( haystacklen < needlelen )	return NULL;

	// try to find a match
	for(begin = (const char *) haystack; begin <= last_possible; begin++){
		if( begin[0] == ((const char *) needle)[0] &&
				!memcmp ((const void *) &begin[1]
					, (const void *) ((const char *) needle + 1)
					, needlelen - 1)){
			return (void *) begin;
		}
	}
	// if no match has been found, return NULL
	return NULL;
#endif
}

NEOIP_NAMESPACE_END



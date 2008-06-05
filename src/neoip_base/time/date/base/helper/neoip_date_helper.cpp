/*! \file
    \brief Definition of static helper functions for the date_t

*/

/* system include */
#include <time.h>
/* local include */
#include "neoip_date_helper.hpp"
#include "neoip_base_oswarp.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			to/from_canonical_string
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the date into a 'canonical' string
 * 
 * - with a possible round up of 1sec
 *   - "struct tm" contain 11 fields... but no msec.... i dont get it
 */
std::string	date_helper_t::to_canonical_string(const date_t &date)		throw()
{
	char		buf[255];
	time_t		nb_sec	= date.to_sec_32bit();
	// convert the time_t to 'struct tm'
#ifndef _WIN32
	struct tm	tm;
	gmtime_r(&nb_sec, &tm);
	// convert the 'struct tm' into a buf
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
#else
	struct tm *	tm;
	tm	= gmtime(&nb_sec);
	// convert the 'struct tm' into a buf
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);	
#endif
	// convert it to a string and return it
	return std::string(buf);
}

/** \brief Convert the date into a 'canonical' string
 */
date_t	date_helper_t::from_canonical_string(const std::string &str)		throw()
{
	time_t 		nb_sec;
	struct tm	st_tm;
#ifndef _WIN32
	// parse the string 
	strptime(str.c_str(), "%Y-%m-%d %H:%M:%S", &st_tm);
#else
	// very special case of strptime :)
	// - mingw doesnt provide strptime
	// - taken from tor source, release under BSD3clause
	unsigned int year=0, month=0, day=0, hour=100, minute=100, second=100;
	if(sscanf(str.c_str(), "%u-%u-%u %u:%u:%u", &year, &month, &day, &hour, &minute, &second) < 6){
		KLOG_ERR("pasring error");
	}
	if(year < 1970 || month < 1 || month > 12 || day < 1 || day > 31 ||
					hour > 23 || minute > 59 || second > 61) {
		KLOG_ERR("ISO time was nonsensical");
	}
	memset(&st_tm, 0, sizeof(st_tm)); 
	st_tm.tm_year	= year-1900;
	st_tm.tm_mon	= month-1;
	st_tm.tm_mday	= day;
	st_tm.tm_hour	= hour;
	st_tm.tm_min	= minute;
	st_tm.tm_sec	= second;
#endif
	// convert the 'struct tm' into a time_t
	nb_sec	= base_oswarp_t::timegm(&st_tm);
	// convert it to a delay_t and return it
	return delay_t::from_sec(nb_sec);
}

NEOIP_NAMESPACE_END;







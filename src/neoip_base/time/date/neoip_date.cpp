/*! \file
    \brief Implementation of the date_t

*/

/* system include */
#include <time.h>
#include <assert.h>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <ctime>
#ifdef _WIN32
#	include <sys/timeb.h>
#endif
/* local include */
#include "neoip_date.hpp"
#include "neoip_date_helper.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a date_t representing the present
 */
date_t date_t::present(void)		throw()
{
	uint64_t	msec;
#ifndef _WIN32
	struct	timeval	tv;
	gettimeofday(&tv, NULL);
	msec 	= (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
#else
	struct _timeb timebuf;
	_ftime (&timebuf);
	msec 	= (uint64_t)timebuf.time * 1000ULL + (uint64_t)timebuf.millitm;
#endif
	return delay_t(msec);
}

/** \brief return true is the date_t is in the past (aka < present)
 */
bool date_t::is_past()			const throw()
{
	return *this < date_t::present();
}

/** \brief return true is the date_t is in the future (aka > present)
 */
bool date_t::is_future()		const throw()
{
	return *this > date_t::present();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			conversion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a date_t in time_t format
 */
time_t date_t::to_time_t()		const throw()
{
	return to_sec_32bit();
}

/** \brief return a date_t in ctime() format
 */
std::string date_t::to_ctime()		const throw()
{
	time_t	tmp	= to_time_t();
#ifndef _WIN32	// TODO not sure this is required, ctime_r is useless for me as i dont do thread
	char	ctime_str[1024];
	return std::string(ctime_r(&tmp, ctime_str));
#else
	return std::string(ctime(&tmp));
#endif
}

/** \brief Convert the date_t to a canonical_string 
 */
std::string	date_t::to_canonical_string()				const throw()
{
	return date_helper_t::to_canonical_string(*this);
}

/** \brief Build a date_t from a canonical_string
 */
date_t		date_t::from_canonical_string(const std::string &str)	throw()
{
	return date_helper_t::from_canonical_string(str);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the date_t into a string
 */
std::string date_t::to_string()	const throw()
{
	std::ostringstream	oss;
	if( is_null() )	return "null";
	if( is_special() ){
		oss << delay_t(*this);
	}else{
		date_t	now	= date_t::present();
#if 1
		if( is_past() ){
			delay_t	delta = now - *this;
			if( delta < delay_t::from_sec(24ULL*60ULL*60ULL) )	oss << delta << " ago";
			else							oss << to_ctime();
		}else if( is_future() ){
			delay_t	delta = *this - now;
			if( delta < delay_t::from_sec(24ULL*60ULL*60ULL) )	oss << "in " << delta;
			else							oss << to_ctime();
		}else{				oss << "now";
		}
#else
		oss << to_ctime();
#endif
	}
	return oss.str();
}

NEOIP_NAMESPACE_END



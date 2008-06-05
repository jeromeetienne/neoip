/*! \file
    \brief Definition of the \ref cpp_backtrace_t (debug only)

\par Brief Description
\ref cpp_backtrace_t stores the stack back trace when the constructor is called.
It allows to display or backup a stack trace.

*/


/* system include */
#ifndef _WIN32
#	include <execinfo.h>
#endif
/* local include */
#include "neoip_cpp_backtrace.hpp"
#include "neoip_cpp_location.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    action function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
cpp_backtrace_t &	cpp_backtrace_t::initialize() 	throw()
{
	void	*array[1000];
#ifndef	_WIN32
	size_t	size	= backtrace( array, sizeof(array) / sizeof(array[0]) );
#else
	size_t	size	= 0;
#endif
	// copy the buffer to the object
	// - start a 1 to skip the call of this function
	for( size_t i = 1; i < size; i++ )
		code_ptr_array.push_back( array[i] );
	// return the object itself
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return a code pointer of the given index
 */
cpp_location_t	cpp_backtrace_t::operator[](int idx)	const
{
	return cpp_location_t( code_ptr_array[idx] );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      ostream redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & oss, const cpp_backtrace_t & cpp_backtrace)	throw()
{
	const std::vector<void *> &		code_ptr_array = cpp_backtrace.code_ptr_array;
	std::vector<void *>::const_iterator	iter;
	for( iter = code_ptr_array.begin(); iter != code_ptr_array.end(); iter++ ){
		const void *code_ptr	= *iter;
		oss << "\n";
		oss << cpp_location_t(code_ptr);
	}
	oss << "\n";
	return oss;
}


NEOIP_NAMESPACE_END


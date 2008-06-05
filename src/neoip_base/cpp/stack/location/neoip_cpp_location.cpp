/*! \file
    \brief Definition of the \ref cpp_location_t (debug only)

\par Brief description
\ref cpp_location_t converts a pointer to a location in the source.
Currently the location is :
-# the executable filename is extracted
-# the function name is extracted and demangled
- the source filename and line number in it arent extracted
  - they may be by using the libbfd from gnu binutils but it isnt currently
    implemented.

*/


/* system include */
#ifndef _WIN32
#	include <execinfo.h>
#endif
/* local include */
#include "neoip_cpp_location.hpp"
#include "neoip_cpp_demangle.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
cpp_location_t::cpp_location_t(const void *ptr) 	throw()
{
#ifndef _WIN32
	std::vector<std::string>	vect_str;
	std::vector<std::string>	vect_str2;
	// get the cooked symbols from the backtrace_symbols() functions and convert to a std::string
	void	*	array_ptr[1];
	array_ptr[0]	= const_cast <void *>(ptr);
	char	**	cooked_c_str	= backtrace_symbols(array_ptr, 1);
	std::string	cooked_str	= cooked_c_str[0];
	free( cooked_c_str );

	// get the executable filename
	vect_str = string_t::split(cooked_str, "( ", 2 );
	// in case of a unknown format in cooked_str, fall back on full unknown
	if( vect_str.size() != 2 ){
		exec_fname	= "unknown";
		function	= "unknown";
		return; 
	}
	
	DBG_ASSERT( vect_str.size() == 2 );
	exec_fname	= vect_str[0];

	// get the function name
	vect_str = string_t::split(vect_str[1], "+", 2 );
	if( vect_str.size() == 2 )	function = neoip_cpp_demangle_function(vect_str[0]);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the executable file name
 */
std::string cpp_location_t::get_exec_fname()	const throw()
{
	return exec_fname;
}

/** \brief return the function name (demangled)
 */
std::string cpp_location_t::get_function_name()	const throw()
{
	return function;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      ostream redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & oss, const cpp_location_t & cpp_location)	throw()
{
	oss << cpp_location.exec_fname << ":";
	if( cpp_location.get_function_name().empty() == false )
		oss << cpp_location.get_function_name();
	else
		oss << "NO_FCT_NAME";
	return oss;
}


NEOIP_NAMESPACE_END


/*! \file
    \brief Some functions to demangle cpp symbols (debug only)

\par Brief Description
This module implements functions to demangle cpp symbol. if the demangle fails
it return the original symbols

\par Poor Coding NOTE :)
- This use libiberty from gnu binutils and go find an unexported function
  cplus_demangle.... couch couch
  - not clean :)
  - but ok for debug
  
*/

/* system include */
#include <string>
#include <cxxabi.h>
#include <cstdlib>
/* local include */
#include "neoip_cpp_demangle.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief demandle a cpp type symbol
 */
std::string	neoip_cpp_demangle_type( const std::string &symbol )	throw()
{
	int status;
	char	*tmp	= abi::__cxa_demangle(symbol.c_str(), 0, 0, &status);
	std::string	demang_symbol;
	// if return NULL, return the orginal symbol
	if( !tmp )	return symbol;	
	demang_symbol	= tmp;
	free(tmp);
	return demang_symbol;
}

/** \brief demandle a cpp function symbol
 */
std::string	neoip_cpp_demangle_function( const std::string &symbol )	throw()
{
	int status;
	char	*tmp	= abi::__cxa_demangle(symbol.c_str(), 0, 0, &status);
	std::string	demang_symbol;
	// if cplus_demangle return NULL, return the orginal symbol
	if( !tmp )	return symbol;	
	demang_symbol	= tmp;
	free(tmp);
	return demang_symbol;
}

NEOIP_NAMESPACE_END

/*! \file
    \brief Header of the neoip_fnmatch
 
\par Brief Description
this is just a workaround because fnmatch is not provided by mingw32.
So a netbsd implementation takes over IIF _WIN32 is defined
    
*/


#ifndef __NEOIP_FNMATCH_HPP__ 
#define __NEOIP_FNMATCH_HPP__ 

#include "neoip_cpp_extern_c.hpp"

#ifndef _WIN32
#	include <fnmatch.h>
#else
NEOIP_CPP_EXTERN_C_BEGIN
	int fnmatch(const char *pattern, const char *string, int flags);
NEOIP_CPP_EXTERN_C_END
#endif

#endif // __NEOIP_FNMATCH_HPP__ 

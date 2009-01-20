/*! \file
    \brief Header of the neoip_errno
    
*/


#ifndef __NEOIP_ERRNO_HPP__ 
#define __NEOIP_ERRNO_HPP__ 
/* system include */
#include <string>
#include <cstring>
#include <cerrno>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief return a std::string from the errno
 * 
 * - this function has been made mainly to able to do "ablabla" + neoip_strerror(errno)
 *   - it wasnt possible to do with the direct use of strerror() as it return a char * and 
 *     not a std::string
 */
inline std::string neoip_strerror( int errno_copy )
{
	return  strerror(errno_copy);
}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ERRNO_HPP__  */




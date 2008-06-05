/*! \file
    \brief Declaration of the lib_apps_oswarp_t

*/


#ifndef __NEOIP_LIB_APPS_OSWARP_HPP__ 
#define __NEOIP_LIB_APPS_OSWARP_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	file_err_t;
class	file_path_t;
class	file_mode_t;
class	file_perm_t;

/** \brief helper to do file operation which depends on the operating system
 */
class	lib_apps_oswarp_t {
public:
	static int	main_for_daemon(const char *apps_name
					, int (*main_internal)(int argc, char *argv[])
					, int argc, char *argv[])		throw();
};
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_APPS_OSWARP_HPP__  */




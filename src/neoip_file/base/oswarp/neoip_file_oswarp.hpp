/*! \file
    \brief Declaration of the file_oswarp_t

*/


#ifndef __NEOIP_FILE_OSWARP_HPP__ 
#define __NEOIP_FILE_OSWARP_HPP__ 
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
class	file_oswarp_t {
public:
	static file_err_t	mkdir(const file_path_t &dir_path
						, const file_perm_t &file_perm)	throw();
	static int		open(const file_path_t &file_path, const file_mode_t &file_mode
						, const file_perm_t &file_perm)	throw();
	static file_path_t	home_dir()					throw();

};
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_OSWARP_HPP__  */




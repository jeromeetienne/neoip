/*! \file
    \brief Header of the \ref file_utils_t.cpp

*/


#ifndef __NEOIP_FILE_UTILS_HPP__ 
#define __NEOIP_FILE_UTILS_HPP__ 

/* system include */
/* local include */
#include "neoip_file_path.hpp"
#include "neoip_file_perm.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to utils() a file
 */
class file_utils_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static const bool	DO_RECURSION;
	static const bool	NO_RECURSION;
public:
	static file_path_t	get_current_dir()				throw();
	static file_path_t	get_home_dir()					throw();
	static file_path_t	get_temp_path(const file_path_t &file_path)	throw();
	static file_err_t	remove_file(const file_path_t &file_path)	throw();
	static file_err_t	create_hardlink(const file_path_t &src_path
						, const file_path_t &dst_path)	throw();
	static file_err_t	create_symlink(const file_path_t &src_path
						, const file_path_t &dst_path)	throw();
	static file_err_t	create_directory(const file_path_t &dir_path, bool recursive
					, const file_perm_t &file_perm = file_perm_t::DIR_DFL)	throw();
	static file_err_t	remove_directory(const file_path_t &dir_path
					, bool recursive = NO_RECURSION)	throw();
	static file_perm_t	umask()						throw();
	static file_err_t	umask(const file_perm_t &file_perm)		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_UTILS_HPP__  */




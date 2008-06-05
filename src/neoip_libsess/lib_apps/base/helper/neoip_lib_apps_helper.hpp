/*! \file
    \brief Header of the neoip_string class
    
*/


#ifndef __NEOIP_LIB_APPS_HELPER_HPP__ 
#define __NEOIP_LIB_APPS_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	libsess_err_t;
class	file_path_t;

/** \brief helper to manipulate string
 */
class lib_apps_helper_t {
public:
	/*************** Misc	***********************************************/
	static libsess_err_t	daemonize()		throw();
	static size_t		current_pid()		throw();
	
	/*************** pidfile create/remove	*******************************/
	static file_path_t	pidfile_path()		throw();
	static void		pidfile_create()	throw();
	static void		pidfile_remove()	throw();

	/*************** url file stuff	***************************************/
	static file_path_t	urlfile_path()		throw();
	static libsess_err_t	urlfile_create()	throw();
	static libsess_err_t	urlfile_remove()	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_APPS_HELPER_HPP__  */




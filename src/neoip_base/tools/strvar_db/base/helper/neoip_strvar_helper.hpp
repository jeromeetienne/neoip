/*! \file
    \brief Header for strvar_helper_t
    
*/


#ifndef __NEOIP_STRVAR_HELPER_HPP__ 
#define __NEOIP_STRVAR_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	strvar_db_t;
class	file_path_t;

/** \brief static helpers to manipulate strvar_db_t
 */
class strvar_helper_t {
public:
	static strvar_db_t	from_file(const file_path_t &file_path)		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_STRVAR_HELPER_HPP__  */











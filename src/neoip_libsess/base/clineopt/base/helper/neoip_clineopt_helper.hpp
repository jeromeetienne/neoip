/*! \file
    \brief Header for handling the convertion of clineopt_t
    
*/


#ifndef __NEOIP_CLINEOPT_HELPER_HPP__ 
#define __NEOIP_CLINEOPT_HELPER_HPP__ 
/* system include */
#include <string>
#include <vector>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


// list of forward declaration
class	clineopt_err_t;
class	clineopt_arr_t;
class	strvar_db_t;

/** \brief static helpers to manipulate swarm_helper in neoip_bt
 */
class clineopt_helper_t {
public:
	static clineopt_err_t	parse(std::vector<std::string> &arg, strvar_db_t &strvar_db
					, const clineopt_arr_t &clineopt_arr)	throw();
	static clineopt_err_t	check(strvar_db_t &strvar_db
					, const clineopt_arr_t &clineopt_arr)	throw();
	static std::string	help_string(const clineopt_arr_t &clineopt_arr)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CLINEOPT_HELPER_HPP__  */











/*! \file
    \brief Header for handling the convertion of ndiag_cacheport_t from/to file
    
*/


#ifndef __NEOIP_NDIAG_CACHEPORT_HELPER_HELPER_HPP__ 
#define __NEOIP_NDIAG_CACHEPORT_HELPER_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_ndiag_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_ndiag_cacheport.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief static helpers to manipulate ndiag_cacheport_t
 */
class ndiag_cacheport_helper_t {
public:
	static ndiag_cacheport_t	from_file(const file_path_t &file_path)		throw();
	static ndiag_err_t	to_file(const ndiag_cacheport_t &ndiag_cacheport
						, const file_path_t &file_path)	throw();
	static file_path_t	default_path()					throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NDIAG_CACHEPORT_HELPER_HELPER_HPP__  */











/*! \file
    \brief Header of the \ref ndiag_err_t

*/


#ifndef __NEOIP_NDIAG_ERR_HPP__ 
#define __NEOIP_NDIAG_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	upnp_err_t;
class	file_err_t;

NEOIP_ERRTYPE_DECLARATION_START(ndiag_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(ndiag_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(ndiag_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(ndiag_err_t	, FROM_UPNP)
NEOIP_ERRTYPE_DECLARATION_ITEM(ndiag_err_t	, FROM_FILE)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(ndiag_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(ndiag_err_t)

// convert a various type of error into a ndiag_err_t
ndiag_err_t ndiag_err_from_upnp(const upnp_err_t &upnp_err, const std::string &prefix_str="")	throw();
ndiag_err_t ndiag_err_from_file(const file_err_t &file_err, const std::string &prefix_str="")	throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_NDIAG_ERR_HPP__  */

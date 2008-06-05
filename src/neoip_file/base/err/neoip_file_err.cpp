/*! \file
    \brief Definition of the \ref file_err_t

*/

/* system include */
/* local include */
#include "neoip_file_err.hpp"
#include "neoip_errno.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ERRTYPE_DEFINITION_START(file_err_t)
NEOIP_ERRTYPE_DEFINITION_ITEM(file_err_t	, OK		, "OK")
NEOIP_ERRTYPE_DEFINITION_ITEM(file_err_t	, ERROR		, "ERROR")
NEOIP_ERRTYPE_DEFINITION_END(file_err_t)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     error convertions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief build a file_err_t from errno
 */
file_err_t file_err_from_errno(const std::string &prefix_str)	throw()
{
	return file_err_t(file_err_t::ERROR, prefix_str + " errno=" + neoip_strerror(errno) );
}
NEOIP_NAMESPACE_END






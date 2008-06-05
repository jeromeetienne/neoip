/*! \file
    \brief Definition of the socket_itor_arg_t

*/

/* system include */
/* local include */
#include "neoip_socket_itor_arg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(socket_itor_arg_t)
NEOIP_ARGPACK_DEF_ITEM(socket_itor_arg_t	, socket_profile_t	, profile)
NEOIP_ARGPACK_DEF_ITEM(socket_itor_arg_t	, socket_domain_t	, domain)
NEOIP_ARGPACK_DEF_ITEM(socket_itor_arg_t	, socket_type_t		, type)
NEOIP_ARGPACK_DEF_ITEM(socket_itor_arg_t	, socket_addr_t		, local_addr)
NEOIP_ARGPACK_DEF_ITEM(socket_itor_arg_t	, socket_addr_t		, remote_addr)
NEOIP_ARGPACK_DEF_CLASS_END(socket_itor_arg_t)

/** \brief Check the validity of the object
 */
bool	socket_itor_arg_t::is_valid()	const throw()
{
	// check for all the mandatory paramters
	if( !domain_present() )		return false;
	if( !type_present() )		return false;
	if( !remote_addr_present() )	return false;
	// return no error
	return true;
}

NEOIP_NAMESPACE_END


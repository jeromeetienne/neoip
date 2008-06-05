/*! \file
    \brief Definition of the tcp_itor_arg_t

*/

/* system include */
/* local include */
#include "neoip_tcp_itor_arg.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(tcp_itor_arg_t)
NEOIP_ARGPACK_DEF_ITEM(tcp_itor_arg_t	, ipport_addr_t		, local_addr)
NEOIP_ARGPACK_DEF_ITEM(tcp_itor_arg_t	, ipport_addr_t		, remote_addr)
NEOIP_ARGPACK_DEF_ITEM(tcp_itor_arg_t	, tcp_profile_t		, profile)
NEOIP_ARGPACK_DEF_CLASS_END(tcp_itor_arg_t)

/** \brief Check the validity of the object
 */
bool	tcp_itor_arg_t::is_valid()	const throw()
{
	// check for all the mandatory parameters
	if( !remote_addr_present() )	return false;
	// return no error
	return true;
}
NEOIP_NAMESPACE_END


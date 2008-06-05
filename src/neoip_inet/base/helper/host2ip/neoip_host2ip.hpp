/*! \file
    \brief Header to choose the various implementation of host2ip_t
    
*/

#ifndef __NEOIP_HOST2IP_HPP__ 
#define __NEOIP_HOST2IP_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

// do the #include of the chosen implementation
#ifdef USE_HOST2IP_FORK
#	include "neoip_host2ip_fork.hpp"
#endif
#ifdef USE_HOST2IP_SYNC
#	include "neoip_host2ip_sync.hpp"
#endif
#ifdef USE_HOST2IP_AGAI
#	include "neoip_host2ip_agai.hpp"
#endif

NEOIP_NAMESPACE_BEGIN;

// do the class definition inheriting of the chosen implementation
#ifdef USE_HOST2IP_FORK
	class	host2ip_t : public host2ip_fork_t {};
#endif
#ifdef USE_HOST2IP_SYNC
	class	host2ip_t : public host2ip_sync_t {};
#endif
#ifdef USE_HOST2IP_AGAI
	class	host2ip_t : public host2ip_agai_t {};
#endif

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HOST2IP_HPP__  */

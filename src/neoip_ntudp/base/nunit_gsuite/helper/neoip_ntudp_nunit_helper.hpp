/*! \file
    \brief Header for the ntudp_nunit_helper.cpp
*/


#ifndef __NEOIP_NTUDP_NUNIT_HELPER_HPP__ 
#define __NEOIP_NTUDP_NUNIT_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_peerid.hpp"
#include "neoip_namespace.hpp"


// list of forward declaration
class ntudp_peer_t;
class ipport_addr_t;

NEOIP_NAMESPACE_BEGIN;

ntudp_err_t	ntudp_nunit_peer_start_inetreach(ntudp_peer_t *ntudp_peer
					, const ipport_addr_t &listen_addr_pview_cfg
					, const ntudp_peerid_t &local_peerid = ntudp_peerid_t()) throw();
ntudp_err_t	ntudp_nunit_peer_start_default(ntudp_peer_t *ntudp_peer)	throw();
ntudp_err_t	ntudp_nunit_peer_start_natsym(ntudp_peer_t *ntudp_peer)		throw();
ntudp_err_t	ntudp_nunit_peer_start_non_natsym(ntudp_peer_t *ntudp_peer)	throw();

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NUNIT_HELPER_HPP__  */




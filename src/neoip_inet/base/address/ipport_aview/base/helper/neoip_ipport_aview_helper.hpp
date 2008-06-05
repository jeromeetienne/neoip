/*! \file
    \brief Declaration of static helper functions for ipport_aview_t
    
*/


#ifndef __NEOIP_IPPORT_AVIEW_HELPER_HPP__ 
#define __NEOIP_IPPORT_AVIEW_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_ipport_aview.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief static helpers for ipport_aview_t
 */
class ipport_aview_helper_t {
public:
	static ipport_aview_t	tcp_listen_aview_from_conf()			throw();
	static ipport_aview_t	udp_listen_aview_from_conf()			throw();
	
	static ipport_addr_t	listen_aview2xmit(const ipport_aview_t &listen_aview
					, const ipport_addr_t &dst_ipport)	throw();
	static ipport_addr_t	listen_xmit2dest(const ipport_addr_t &xmit_listen_ipport
					, const ipport_addr_t &src_ipport)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPPORT_AVIEW_HELPER_HPP__  */











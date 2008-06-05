/*! \file
    \brief Definition of static helper functions for the ipport_addr_t

*/

/* system include */
/* local include */
#include "neoip_ipport_addr_helper.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_tcp_resp.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			find_avail_listen_addr
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Try to find a available listen_addr in the range given
 */
ipport_addr_t	ipport_addr_helper_t::find_avail_listen_addr(const ip_addr_t &ip_addr
				, const uint16_t &min_port, const uint16_t &max_port)	throw()
{
	inet_err_t	inet_err;
	tcp_resp_t *	tcp_resp;
	// go thru all the port
	for(uint16_t cur_port = min_port; cur_port <= max_port; cur_port++){
		// build the listen address
		ipport_addr_t	listen_ipport(ip_addr, cur_port);
		// log to debug
		KLOG_ERR("listen_ipport=" << listen_ipport);
		// try to start a tcp_resp_t on the cur_ipport
		tcp_resp	= nipmem_new tcp_resp_t();
		inet_err	= tcp_resp->start(listen_ipport, NULL, NULL);
		nipmem_zdelete	tcp_resp;
		// if the start() failed, goto the next
		if( inet_err.failed() )	continue;
		// if this listen_ipport succeed, return it
		return listen_ipport;
		
	}
	// if this point is reached, none is available, return a null ipport_addr_t
	return ipport_addr_t();
}

NEOIP_NAMESPACE_END;







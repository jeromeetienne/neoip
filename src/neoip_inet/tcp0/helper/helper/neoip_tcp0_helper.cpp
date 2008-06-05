/*! \file
    \brief Definition of static helper functions for the tcp layer

*/

/* system include */
/* local include */
#include "neoip_tcp_helper.hpp"
#include "neoip_tcp_resp.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         TODO to comment
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief get a free listen ipport from the lib_session_t config (without actually binding it)
 * 
 * - TODO to refactor and to comment
 *   - i coded this function while having no brain, seems to work but likely an illusion
 *   - there is a very similar one in udp_helper_t and in tcp_helper_t
 * 
 * @return in case of error, the pair.first is a null ipport_addr_t
 */
std::pair<ipport_addr_t, ipport_addr_t>	tcp_helper_t::get_free_listen_pair_from_conf()	throw()
{
	const strvar_db_t &session_conf	= lib_session_get()->session_conf();
	ip_addr_t	lview_addr	= session_conf.get_first_value("tcp_listen_lview_addr","0.0.0.0");
	uint16_t	lview_port_min	= atoi(session_conf.get_first_value("tcp_listen_lview_port_min","0").c_str());
	uint16_t	lview_port_max	= atoi(session_conf.get_first_value("tcp_listen_lview_port_max","0").c_str());
	ip_addr_t	pview_addr	= session_conf.get_first_value("tcp_listen_pview_addr","0.0.0.0");
	uint16_t	pview_port_min	= atoi(session_conf.get_first_value("tcp_listen_pview_port_min","0").c_str());
	uint16_t	pview_port_max	= atoi(session_conf.get_first_value("tcp_listen_pview_port_max","0").c_str());
	ipport_addr_t	lview_ipport;
	ipport_addr_t	pview_ipport;
	
	// check the size of the pview_port range is the same as the lview_port range
	if( pview_port_max && lview_port_max-lview_port_min != pview_port_max-pview_port_min){
		KLOG_ERR("ERROR in lib_session.conf. tcp_listen_pview port range is not the same size as the lview port range.");
		return std::make_pair(ipport_addr_t(), ipport_addr_t());
	}

	// go thru the whole ipport_inval
	uint16_t	chosen_port;
	for(chosen_port = lview_port_min; chosen_port <= lview_port_max && lview_port_max; chosen_port++){
		ipport_addr_t	cur_ipport(lview_addr, chosen_port);
		inet_err_t	inet_err;
		tcp_resp_t *	tcp_resp;
		// try to start a tcp_resp_t on the cur_ipport
		tcp_resp	= nipmem_new tcp_resp_t();
		inet_err	= tcp_resp->start(cur_ipport, NULL, NULL);
		nipmem_zdelete	tcp_resp;
		// if the start() succeed, leave the loop
		if( inet_err.succeed() )	break;
	}
	// compute the lview_ipport/pview_ipport depending on the chosen_port
	if( chosen_port <= lview_port_max && lview_port_max){
		// set lview_ipport with the chosen_port
		lview_ipport	= ipport_addr_t(lview_addr, chosen_port);
		// set pview_ipport with the port with the same delta but in pview_port_min/max
		pview_ipport	= ipport_addr_t(pview_addr, pview_port_min+(chosen_port-lview_port_min));
	}else{	// if no predefined port has been found
		// set lview_ipport with ANY port
		lview_ipport	= ipport_addr_t(lview_addr, 0);
		// set pview_ipport with a null ipport_addr_t 
		pview_ipport	= ipport_addr_t();
	}

	// return the result
	return std::make_pair(lview_ipport, pview_ipport);
}


NEOIP_NAMESPACE_END;







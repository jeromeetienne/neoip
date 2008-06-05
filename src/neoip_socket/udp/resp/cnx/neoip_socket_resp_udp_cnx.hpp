/*! \file
    \brief Declaration of the neoip_socket_udp_dresp
*/


#ifndef __NEOIP_SOCKET_RESP_UDP_CNX_HPP__
#define __NEOIP_SOCKET_RESP_UDP_CNX_HPP__
/* system include */
/* local include */
#include "neoip_socket_resp_udp.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief handle a given udp connection received by socket_resp_udp_t
 * 
 * - it perform a nlay_resp_t on top of a udp_full_t
 */
class socket_resp_udp_t::cnx_t : private udp_full_cb_t {
private:
	socket_resp_udp_t *	socket_resp_udp;	//!< backpointer on the socket_resp_udp_t

	/*************** udp_full_t stuff	*******************************/
	udp_full_t *	udp_full;		//!< udp_full_t struct
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw();

	/*************** Internal function	*******************************/
	bool		recv_udp_pkt(pkt_t &pkt)			throw();
	bool		notify_cnx_established(nlay_full_t *nlay_full)	throw();
public:
	/*************** ctor/dtor	***************************************/
	cnx_t(socket_resp_udp_t *socket_resp_udp, udp_full_t *udp_full)	throw();
	~cnx_t()							throw();

	/*************** Setup function	***************************************/
	socket_err_t	start()						throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_RESP_UDP_CNX_HPP__ */




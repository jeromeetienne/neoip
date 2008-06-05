/*! \file
    \brief Declaration of the neoip_socket_udp_dresp
*/


#ifndef __NEOIP_SOCKET_RESP_STCP_CNX_HPP__
#define __NEOIP_SOCKET_RESP_STCP_CNX_HPP__
/* system include */
/* local include */
#include "neoip_socket_resp_stcp.hpp"
#include "neoip_tcp_full_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	slay_resp_t;
class	slay_full_t;

/** \brief handle a given tcp connection received by socket_resp_stcp_t
 */
class socket_resp_stcp_cnx_t : private tcp_full_cb_t {
private:
	socket_resp_stcp_t *	m_resp_stcp;	//!< backpointer on the socket_resp_stcp_t
	slay_resp_t *		m_slay_resp;	//!< slay context for full

	/*************** Internal function	*******************************/
	void			xmit_ifneeded()					throw();
	bool			spawn_socket_full(slay_full_t *slay_full)	throw();
	bool			autodelete(const std::string &reason = "")	throw();
	
	/*************** tcp_full_t stuff	*******************************/
	tcp_full_t *		m_tcp_full;		//!< tcp_full_t struct
	bool			neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
						, const tcp_event_t &tcp_event)	throw();
	bool			handle_recved_data(pkt_t &pkt)			throw();
	bool			handle_maysend_on()				throw();
public:
	/*************** ctor/dtor	***************************************/
	socket_resp_stcp_cnx_t(socket_resp_stcp_t *p_resp_stcp, tcp_full_t *p_tcp_full)	throw();
	~socket_resp_stcp_cnx_t()							throw();

	/*************** Setup function	***************************************/
	socket_err_t	start()				throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_RESP_STCP_CNX_HPP__ */




/*! \file
    \brief Declaration of the neoip_socket_udp_dresp
*/


#ifndef __NEOIP_SOCKET_RESP_NTLAY_CNX_HPP__
#define __NEOIP_SOCKET_RESP_NTLAY_CNX_HPP__
/* system include */
/* local include */
#include "neoip_socket_resp_ntlay.hpp"
#include "neoip_ntudp_full_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief handle a given udp connection received by socket_resp_ntlay_t
 * 
 * - it perform a nlay_resp_t on top of a ntudp_full_t
 */
class socket_resp_ntlay_t::cnx_t : private ntudp_full_cb_t {
private:
	socket_resp_ntlay_t *	socket_resp_ntlay;	//!< backpointer on the socket_resp_ntlay_t

	/*************** ntudp_full_t stuff	*******************************/
	ntudp_full_t *	ntudp_full;		//!< ntudp_full_t struct
	bool		neoip_ntudp_full_event_cb(void *userptr, ntudp_full_t &cb_ntudp_full
						, const ntudp_event_t &ntudp_event)	throw();

	/*************** Internal function	*******************************/
	bool		handle_recved_data(pkt_t &pkt)			throw();
	bool		notify_cnx_established(nlay_full_t *nlay_full)	throw();
public:
	/*************** ctor/dtor	***************************************/
	cnx_t(socket_resp_ntlay_t *socket_resp_ntlay, ntudp_full_t *ntudp_full)	throw();
	~cnx_t()								throw();

	/*************** Setup function	***************************************/
	socket_err_t	start()							throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_RESP_NTLAY_CNX_HPP__ */




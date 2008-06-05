/*! \file
    \brief Declaration of the ntudp_resp_direct_t
    
*/


#ifndef __NEOIP_NTUDP_RESP_DIRECT_HPP__ 
#define __NEOIP_NTUDP_RESP_DIRECT_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_resp_direct_cb.hpp"
#include "neoip_ntudp_addr.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_pkt.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_resp_t;
class	ipport_addr_t;

/** \brief Perform a saddrecho request and notify the result
 */
class ntudp_resp_direct_t : NEOIP_COPY_CTOR_DENY, private udp_full_cb_t, private zerotimer_cb_t {
private:
	ntudp_resp_t *	ntudp_resp;	//!< backpointer on the ntudp_resp_t
	ntudp_nonce_t	client_nonce;	//!< the client nonce included in each request pkt
	ntudp_addr_t	m_local_addr;	//!< the local ntudp_addr_t
	ntudp_addr_t	m_remote_addr;	//!< the remote ntudp_addr_t
	pkt_t		pkt_request;	//!< the request packet stored between the creation of the
					//!< object and the zerotimer expiration for its parsing
					

	/*************** zerotimer	***************************************/
	zerotimer_t	zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw();

	/*************** Callback	***************************************/
	ntudp_resp_direct_cb_t*callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(udp_full_t *udp_full, const pkt_t &estapkt_in
						, const pkt_t &estapkt_out)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

	/*************** udp_client	***************************************/
	udp_full_t *	udp_full;
	bool		neoip_inet_udp_full_event_cb(void *cb_userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw();
	pkt_t 		build_direct_cnx_reply()	const throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_resp_direct_t(ntudp_resp_t *ntudp_resp, const pkt_t &pkt_request, udp_full_t *udp_full
				, ntudp_resp_direct_cb_t *callback, void * userptr)	throw();
	~ntudp_resp_direct_t()				throw();
	
	/*************** Query Function	***************************************/
	const ntudp_addr_t &	local_addr()	const throw()	{ return m_local_addr;	}
	const ntudp_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RESP_DIRECT_HPP__  */




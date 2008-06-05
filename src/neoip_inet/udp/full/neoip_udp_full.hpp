/*! \file
    \brief Declaration of the udp_full_t
    
*/


#ifndef __NEOIP_UDP_FULL_HPP__ 
#define __NEOIP_UDP_FULL_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_udp_full_cb.hpp"
#include "neoip_udp_full_wikidbg.hpp"
#include "neoip_socket_mtu_vapi.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_udp_event.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_timeout.hpp"
#include "neoip_object_slotid.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief class definition for udp_full_t
 */
class udp_full_t : NEOIP_COPY_CTOR_DENY, private fdwatch_cb_t, private zerotimer_cb_t
				, private timeout_cb_t, public object_slotid_t
				, public socket_mtu_vapi_t
				, private wikidbg_obj_t<udp_full_t, udp_full_wikidbg_init>
				{
private://///////////////////// constant declaration ///////////////////////////
	// the maximum IPv4 datagram length
	static const size_t	MAX_IP4_DGRAM;
	// the minimum IPv4 header length
	static const size_t	IP_HEADER_MIN_LEN;
	// the udp header length
	static const size_t	UDP_HEADER_LEN;
	// the maximum length of a udp payload
	static const size_t	UDP_PAYLOAD_MAX_LEN;
private:
	int		sock_fd;	//!< to bind the socket during the setup functions (if == 0
					//!< the sock_fd belong to fd_watch)
	ipport_addr_t	local_addr;	//!< the local address
	ipport_addr_t	remote_addr;	//!< the remote address

	
	size_t		recv_max_len;	//!< the current recv_max_len

	/*************** fdwatch stuff	***************************************/
	fdwatch_t *	fdwatch;
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** mtu growth stuff	*******************************/
	size_t		m_mtu_outter;		//!< local copy of current mtu outter (may be desync with kernel one) 
	size_t		mtu_outter_from_kernel()		const throw();
	delaygen_t	mtugrow_delaygen;	//!< the delaygen_t for the mtugrow_timeout
	timeout_t	mtugrow_timeout;	//!< to detect growth in the mtu
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
			
	/*************** internal function	*******************************/
	inet_err_t	create_sock_fd_if_needed()				throw();
	void		purge_msg_errqueue()					throw();

	/*************** function for friend class	***********************/
	inet_err_t	set_local_addr(const ipport_addr_t &local_addr)		throw();
	inet_err_t	set_remote_addr(const ipport_addr_t &remote_addr)	throw();

	/*************** resp_dgram_zerotimer	*******************************/
	zerotimer_t	resp_dgram_zerotimer;	//!< used to delay the pushed back pkt from the udp_resp_t
	std::list<pkt_t>resp_dgram_db;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** callback stuff	***************************************/
	udp_full_cb_t *	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const udp_event_t &udp_event)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	udp_full_t()			throw();
	~udp_full_t()			throw();

	/*************** null function	***************************************/
	bool		is_null()	const throw();
	void		nullify()	throw();

	/*************** setup function	***************************************/
	inet_err_t		set_callback(udp_full_cb_t *callback, void *userptr)	throw();
	inet_err_t		start()						throw();
	inet_err_t		start(udp_full_cb_t *callback, void *userptr)	throw();

	/*************** socket_mtu_vapi_t	*******************************/
	void			mtu_pathdisc(bool onoff)	throw();
	bool			mtu_pathdisc()			const throw();
	size_t			mtu_overhead()			const throw();
	size_t			mtu_outter()			const throw()	{ return m_mtu_outter;			}
	size_t			mtu_inner()			const throw()	{ return mtu_outter() - mtu_overhead();	}

	// TODO should be ported to new API with compatibility layer
	const ipport_addr_t &	get_local_addr()				const throw();
	const ipport_addr_t &	get_remote_addr()				const throw();
	inet_err_t		recv_max_len_set(size_t recv_max_len = UDP_PAYLOAD_MAX_LEN)	throw();
	size_t			recv_max_len_get()				const throw();
		

	
	// send a datagram
	size_t			send(const void *pkt_data, size_t pkt_len) 		throw();
	/*************** Helper function	*******************************/
	size_t	send(const pkt_t &pkt)		throw()	{ return send(pkt.get_data(), pkt.get_len());	}
	size_t	send(const datum_t &datum)	throw()	{ return send(datum.get_data(), datum.get_len());}

	// internal function - made private to be accessible to udp_vresp_t::cnx_t
	// - TODO remove this from the public: and make the udp_vresp_cnx_t as friend
	void		push_back_resp_dgram(const void *pkt_ptr, int pkt_len)	throw();

	/*************** Display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const udp_full_t &udp_full)	throw()
						{ return os << udp_full.to_string();	}	

	/*************** Friend class	***************************************/
	friend class udp_resp_t;
	friend class udp_itor_t;
	friend class udp_full_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_FULL_HPP__  */




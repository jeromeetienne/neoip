/*! \file
    \brief Header of the ntudp_client_t
*/


#ifndef __NEOIP_NTUDP_CLIENT_HPP__ 
#define __NEOIP_NTUDP_CLIENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_itor_retry.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_client_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the udp client
 * 
 * - it is a helper class which merge the use of ntudp_itor_t and ntudp_full_t
 *   which are typical for ntudp clients.
 */
class ntudp_client_t : NEOIP_COPY_CTOR_DENY, private ntudp_itor_retry_cb_t, private ntudp_full_cb_t
					, public socket_mtu_vapi_t {
private:
	ntudp_peer_t *	ntudp_peer;	//!< the ntudp_peer_t on which this client is attached
	/*************** ntudp_itor_t	***************************************/
	ntudp_itor_retry_t *	ntudp_itor;	//!< to initiate the connection
	bool		neoip_ntudp_itor_retry_event_cb(void *userptr, ntudp_itor_retry_t &cb_itor_retry
						, const ntudp_event_t &ntudp_event)	throw();

	/*************** ntudp_full_t	***************************************/
	ntudp_full_t *	ntudp_full;	//!< the ntudp_full_t struct
	bool		neoip_ntudp_full_event_cb(void *userptr, ntudp_full_t &cb_ntudp_full
						, const ntudp_event_t &ntudp_event)	throw();
	bool		recv_cnx_established(ntudp_full_t *ntudp_full)		throw();
	
	/*************** callback	***************************************/
	ntudp_client_cb_t *	callback;	//!< the callback to notify in case of event
	void *			userptr;	//!< the userptr associated with the event callback
	bool			notify_callback(const ntudp_event_t &ntudp_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_client_t() 	throw();
	~ntudp_client_t() 	throw();

	/*************** setup function	***************************************/
	void		set_callback(ntudp_client_cb_t *callback, void *userptr)	throw();
	ntudp_err_t	start()								throw();
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &remote_addr
					, ntudp_client_cb_t *callback, void * userptr)	throw();

	//! return if ntudp_client_t is in itor phase, false otherwise
	bool		is_itor()	const throw()	{ return ntudp_itor ? true : false;	}
	ntudp_full_t *	steal_full()	throw();

	/*************** Set local/remote addresses - itor only	***************/
	ntudp_client_t&	set_retry_event(const ntudp_event_t::type &event_type)	throw()
			{ DBG_ASSERT(ntudp_itor); ntudp_itor->set_retry_event(event_type); return *this;}
	ntudp_client_t &set_local_addr(const ntudp_addr_t &local_addr)			throw()
			{ DBG_ASSERT(ntudp_itor); ntudp_itor->set_local_addr(local_addr); return *this;	}
	ntudp_err_t	set_remote_addr(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &remote_addr) throw()
			{ DBG_ASSERT(ntudp_itor); return ntudp_itor->set_remote_addr(ntudp_peer, remote_addr);	}

	/*************** Get local/remote addresses - itor or full	*******/
	ntudp_addr_t		local_addr()	const throw()
					{ if(ntudp_itor)	return ntudp_itor->local_addr();
					  else			return ntudp_full->local_addr();	}
	const ntudp_addr_t &	remote_addr()	const throw()
					{ if(ntudp_itor)	return ntudp_itor->remote_addr();
					  else			return ntudp_full->remote_addr();	}

	/*************** send function - ntudp_full only	***********************/
	size_t	send(const void *data_ptr, size_t data_len) 		throw()
				{ DBG_ASSERT(ntudp_full); return ntudp_full->send(data_ptr, data_len);	}
	size_t	send(const pkt_t &pkt) 					throw()
				{ DBG_ASSERT(ntudp_full); return ntudp_full->send(pkt);	}
	size_t	send(const datum_t &datum) 				throw()
				{ DBG_ASSERT(ntudp_full); return ntudp_full->send(datum);	}

	/*************** socket_mtu_vapi_t accessor	***********************/
	const socket_mtu_vapi_t*mtu_vapi()		const throw()	{ DBG_ASSERT(ntudp_full); return ntudp_full;}
	socket_mtu_vapi_t *	mtu_vapi()		throw()		{ DBG_ASSERT(ntudp_full); return ntudp_full;}
	/*************** socket_mtu_vapi_t	*******************************/
	void			mtu_pathdisc(bool onoff)throw()		{ mtu_vapi()->mtu_pathdisc(onoff);	}
	bool			mtu_pathdisc()		const throw()	{ return mtu_vapi()->mtu_pathdisc();	}
	size_t			mtu_overhead()		const throw()	{ return mtu_vapi()->mtu_overhead();	}
	size_t			mtu_outter()		const throw()	{ return mtu_vapi()->mtu_outter();	}
	size_t			mtu_inner()		const throw()	{ return mtu_vapi()->mtu_inner();	}

	/*************** various function - ntudp_full only	***************/
	ntudp_err_t	recv_max_len_set(size_t recv_max_len)	throw()
			{ DBG_ASSERT(ntudp_full); return ntudp_full->recv_max_len_set(recv_max_len);	}
	size_t		recv_max_len_get()			const throw()
			{ DBG_ASSERT(ntudp_full); return ntudp_full->recv_max_len_get();		}

	/*************** display function	*******************************/
	std::string	to_string()			const throw();	
	friend	std::ostream & operator << (std::ostream & os, const ntudp_client_t &ntudp_client)	throw()
							{ return os << ntudp_client.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_CLIENT_HPP__  */




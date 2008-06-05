/*! \file
    \brief Declaration of the ntudp_full_t
    
*/


#ifndef __NEOIP_NTUDP_FULL_HPP__ 
#define __NEOIP_NTUDP_FULL_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_ntudp_full_cb.hpp"
#include "neoip_ntudp_full_wikidbg.hpp"
#include "neoip_ntudp_addr.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_object_slotid.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_event_t;

/** \brief class definition for ntudp_full_t
 */
class ntudp_full_t  : NEOIP_COPY_CTOR_DENY, private udp_full_cb_t, public object_slotid_t
				, public socket_mtu_vapi_t
				, private wikidbg_obj_t<ntudp_full_t, ntudp_full_wikidbg_init>
				{
private:
	ntudp_peer_t *	ntudp_peer;	//!< backpointer on the attached ntudp_peer_t
	ntudp_addr_t	m_local_addr;	//!< the local address
	ntudp_addr_t	m_remote_addr;	//!< the remote address
	pkt_t		estapkt_in;	//!< the incoming packet from the connection establishement
	pkt_t		estapkt_out;	//!< the outgoing packet from the connection establishement

	/*************** udp_full_t	***************************************/
	udp_full_t *	udp_full;	//!< the udp_full_t on which this connection occurs
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw();
	bool		handle_recved_data(pkt_t *pkt)	throw();

	/*************** callback	***************************************/
	ntudp_full_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const ntudp_event_t &ntudp_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_full_t(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &m_local_addr
					, const ntudp_addr_t &m_remote_addr, udp_full_t *udp_full) throw();
	~ntudp_full_t()			throw();
	
	/*************** Setup function	***************************************/
	ntudp_full_t &	set_estapkt(const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
	ntudp_full_t &	set_callback(ntudp_full_cb_t *callback, void *userptr)		throw();
	ntudp_err_t	start()								throw();
	ntudp_err_t	start(ntudp_full_cb_t *callback, void *userptr)			throw();

	/*************** Query function	***************************************/
	ntudp_peer_t *		get_ntudp_peer()	throw()		{ return ntudp_peer;	}
	const ntudp_addr_t &	local_addr()		const throw()	{ return m_local_addr;	}
	const ntudp_addr_t &	remote_addr()		const throw()	{ return m_remote_addr;	}

	/*************** send function ****************************************/
	size_t	send(const void *data_ptr, size_t data_len) 		throw()
				{ DBG_ASSERT(udp_full); return udp_full->send(data_ptr, data_len);	}
	size_t	send(const pkt_t &pkt) 					throw()
				{ DBG_ASSERT(udp_full); return udp_full->send(pkt);			}
	size_t	send(const datum_t &datum) 				throw()
				{ DBG_ASSERT(udp_full); return udp_full->send(datum);			}

	/*************** socket_mtu_vapi_t accessor	***********************/
	const socket_mtu_vapi_t*mtu_vapi()		const throw()	{ DBG_ASSERT(udp_full); return udp_full;}
	socket_mtu_vapi_t *	mtu_vapi()		throw()		{ DBG_ASSERT(udp_full); return udp_full;}
	/*************** socket_mtu_vapi_t	*******************************/
	void			mtu_pathdisc(bool onoff)throw()		{ mtu_vapi()->mtu_pathdisc(onoff);	}
	bool			mtu_pathdisc()		const throw()	{ return mtu_vapi()->mtu_pathdisc();	}
	size_t			mtu_overhead()		const throw()	{ return mtu_vapi()->mtu_overhead();	}
	size_t			mtu_outter()		const throw()	{ return mtu_vapi()->mtu_outter();	}
	size_t			mtu_inner()		const throw()	{ return mtu_vapi()->mtu_inner();	}

	/*************** various function - udp_full only	***************/
	size_t		recv_max_len_get()			const throw()
				{ return udp_full->recv_max_len_get();				}
	ntudp_err_t	recv_max_len_set(size_t recv_max_len)	throw()
				{ return ntudp_err_from_inet( udp_full->recv_max_len_set(recv_max_len) );}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const ntudp_full_t &ntudp_full ) throw()
					{ return os << ntudp_full.to_string();	}
		
	/*************** Friend class	***************************************/
	friend class ntudp_full_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_FULL_HPP__  */




/*! \file
    \brief Header of the udp_client_t
*/


#ifndef __NEOIP_UDP_CLIENT_HPP__ 
#define __NEOIP_UDP_CLIENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_udp_client_cb.hpp"
#include "neoip_udp_client_wikidbg.hpp"
#include "neoip_udp_itor.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class udp_itor_t;
class udp_full_t;

/** \ref class to store the udp client
 * 
 * - it is a helper class which merge the use of udp_itor_t and udp_full_t
 *   which are typical for udp clients.
 */
class udp_client_t : NEOIP_COPY_CTOR_DENY, private udp_itor_cb_t, private udp_full_cb_t
			, public socket_mtu_vapi_t
			, private wikidbg_obj_t<udp_client_t, udp_client_wikidbg_init>
			{
private:
	/*************** udp initiator	***************************************/
	udp_itor_t *	udp_itor;	//!< to initiate the connection
	bool		neoip_inet_udp_itor_event_cb(void *userptr, udp_itor_t &cb_udp_itor
						, const udp_event_t &udp_event)	throw();

	/*************** udp full	***************************************/
	udp_full_t *	udp_full;	//!< the udp_full_t struct
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw();
	bool		recv_cnx_established(udp_full_t *udp_full)		throw();
	
	/*************** callback	***************************************/
	udp_client_cb_t *	callback;	//!< the callback to notify in case of event
	void *			userptr;	//!< the userptr associated with the event callback
	bool			notify_callback(const udp_event_t &udp_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	udp_client_t() 	throw();
	~udp_client_t() throw();

	/*************** setup function	***************************************/
	inet_err_t	set_callback(udp_client_cb_t *callback, void *userptr)	throw();
	inet_err_t	start()							throw()
			{ DBG_ASSERT(udp_itor); return udp_itor->start();		}
	inet_err_t	start(const ipport_addr_t &remote_addr, udp_client_cb_t *callback
						, void * userptr)		throw();

	/*************** Query function	***************************************/
	bool		is_itor()	const throw()	{ return udp_itor ? true : false;	}

	udp_full_t *	steal_full()	throw();

	/*************** Set local/remote addresses - itor only	***************/
	inet_err_t		local_addr(const ipport_addr_t &m_local_addr)		throw()
					{ DBG_ASSERT(udp_itor); return udp_itor->set_local_addr(m_local_addr);	}
	inet_err_t		remote_addr(const ipport_addr_t &m_remote_addr)		throw()
					{ DBG_ASSERT(udp_itor); return udp_itor->set_remote_addr(m_remote_addr);}

	/*************** Get local/remote addresses - itor or full	*******/
	const ipport_addr_t &	local_addr()	const throw()
					{ if(udp_itor)	return udp_itor->get_local_addr();
					  else		return udp_full->get_local_addr();	}
	const ipport_addr_t &	remote_addr()	const throw()
					{ if(udp_itor)	return udp_itor->get_remote_addr();
					  else		return udp_full->get_remote_addr();	}

	/*************** Compatibility layer	*******************************/
	const ipport_addr_t &	get_local_addr()	const throw()	{ return local_addr();	}
	const ipport_addr_t &	get_remote_addr()	const throw()	{ return remote_addr();	}
	inet_err_t		set_local_addr(const ipport_addr_t &m_local_addr)	throw()
							{ return local_addr(m_local_addr);	}
	inet_err_t		set_remote_addr(const ipport_addr_t &m_remote_addr)	throw()
							{ return remote_addr(m_remote_addr);	}

	/*************** send function - udp_full only	***********************/
	size_t	send(const void *data_ptr, size_t data_len) 		throw()
				{ DBG_ASSERT(udp_full); return udp_full->send(data_ptr, data_len);	}
	size_t	send(const pkt_t &pkt) 					throw()
				{ DBG_ASSERT(udp_full); return udp_full->send(pkt);	}
	size_t	send(const datum_t &datum) 				throw()
				{ DBG_ASSERT(udp_full); return udp_full->send(datum);	}

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
	inet_err_t	recv_max_len_set(size_t recv_max_len)	throw()
			{ DBG_ASSERT(udp_full); return udp_full->recv_max_len_set(recv_max_len);	}
	size_t		recv_max_len_get()		const throw()
			{ DBG_ASSERT(udp_full); return udp_full->recv_max_len_get();			}

	/*************** display function	*******************************/
	std::string	to_string()		const throw();	
	friend	std::ostream & operator << (std::ostream & os, const udp_client_t &udp_client)	throw()
						{ return os << udp_client.to_string();	}

	/*************** Friend class	***************************************/
	friend class	udp_client_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_CLIENT_HPP__  */




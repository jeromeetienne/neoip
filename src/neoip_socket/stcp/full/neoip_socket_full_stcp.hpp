/*! \file
    \brief Declaration of the neoip_socket_stcp_dfull
*/


#ifndef __NEOIP_SOCKET_FULL_STCP_HPP__
#define __NEOIP_SOCKET_FULL_STCP_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_full_stcp_wikidbg.hpp"
#include "neoip_socket_helper_stcp.hpp"
#include "neoip_socket_full_vapi.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_tcp_full_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_full_t;
class	slay_full_t;

/** \brief class for the \ref socket_full_t of the socket_domain_t::STCP
 */
class socket_full_stcp_t : NEOIP_COPY_CTOR_DENY, private tcp_full_cb_t
			, public socket_full_vapi_t, public zerotimer_cb_t
			, private wikidbg_obj_t<socket_full_stcp_t, socket_full_stcp_wikidbg_init, socket_full_vapi_t>
			{
private:
	slay_full_t *		m_slay_full;		//!< slay context for full
	socket_type_t		m_socket_type;		//!< the socket_type_t for this initiator
	socket_profile_t	m_socket_profile;	//!< the socket_profile_t for this socket
	socket_addr_t		m_local_addr;		//!< the local socket_addr_t
	socket_addr_t		m_remote_addr;		//!< the remote socket_addr_t

	/*************** initflush_zerotimer	*******************************/
	zerotimer_t		m_initflush_zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** tcp_full_t stuff	*******************************/
	tcp_full_t *		m_tcp_full;
	bool			neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_stcp_full
						, const tcp_event_t &tcp_event)	throw();
	bool			handle_recved_data(pkt_t &pkt)			throw();

	/*************** callback stuff	***************************************/
	socket_full_vapi_cb_t *	callback;	//!< callback to notify event
	void *			userptr;	//!< userptr associated with the event callback
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

	/*************** Setup function	***************************************/
	socket_err_t	set_ctor_param(tcp_full_t *p_tcp_full, slay_full_t *p_slay_full
				, const socket_profile_t &socket_profile)	throw();
public:
	/*************** ctor/dtor	***************************************/
	socket_full_stcp_t()							throw();
	~socket_full_stcp_t()							throw();
	
	// function inherited from socket_full_vapi_t
	void		set_callback(socket_full_vapi_cb_t *callback, void *userptr)	throw();
	socket_err_t	start()								throw();


	void		notify_glue_destruction()			throw();
	void		rcvdata_maxlen(size_t new_value)		throw();
	size_t		rcvdata_maxlen()				const throw();
	size_t		send(const void *data_ptr, size_t data_len) 	throw();

	/*************** various get	***************************************/
	const socket_domain_t &	domain()	const throw()	{ return socket_helper_stcp_t::DOMAIN_VAR;}
	const socket_type_t &	type()		const throw()	{ return m_socket_type;		}		
	const socket_profile_t &profile()	const throw()	{ return m_socket_profile;	}
	const socket_addr_t &	local_addr()	const throw()	{ return m_local_addr;		}
	const socket_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;		}

	/*************** socket_stream_vapi_t accessor	***********************/
	const socket_stream_vapi_t*stream_vapi()	const throw();
	socket_stream_vapi_t *	stream_vapi()		throw();
	bool			stream_vapi_avail()	const throw()	{ return true;	}
	
	/*************** socket_rate_vapi_t accessor	***********************/
	const socket_rate_vapi_t*rate_vapi()		const throw();
	socket_rate_vapi_t *	rate_vapi()		throw();
	bool			rate_vapi_avail()	const throw()	{ return true;	}
	
	/*************** Display function	*******************************/
	std::string	to_string()					const throw();	

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(socket_full_vapi_t, socket_full_stcp_t);
	
	/*************** List of friend class	*******************************/
	friend class	socket_itor_stcp_t;
	friend class	socket_resp_stcp_t;
	friend class	socket_resp_stcp_cnx_t;
	friend class	socket_full_stcp_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_FULL_STCP_HPP__ */




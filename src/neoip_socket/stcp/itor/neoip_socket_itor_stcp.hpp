/*! \file
    \brief Declaration of the \ref socket_itor_stcp_t
*/


#ifndef __NEOIP_SOCKET_ITOR_STCP_HPP__
#define __NEOIP_SOCKET_ITOR_STCP_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_itor_stcp_wikidbg.hpp"
#include "neoip_socket_helper_stcp.hpp"
#include "neoip_socket_itor_vapi.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_tcp_client_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_itor_t;
class	slay_itor_t;
class	slay_full_t;
class	slay_err_t;

/** \brief class for the \ref socket_itor_t of the TCP domain
 */
class socket_itor_stcp_t : public socket_itor_vapi_t, public tcp_client_cb_t
			, private wikidbg_obj_t<socket_itor_stcp_t, socket_itor_stcp_wikidbg_init, socket_itor_vapi_t>
			{
private:
	slay_itor_t *		m_slay_itor;		//!< slay context for itor
	socket_type_t		m_socket_type;		//!< the socket_type_t for this initiator
	socket_profile_t	m_socket_profile;	//!< the socket_profile_t for this socket
	socket_addr_t		m_local_addr;		//!< the local socket_addr_t
	socket_addr_t		m_remote_addr;		//!< the remote socket_addr_t

	/*************** Internal function	*******************************/
	void			xmit_ifneeded()					throw();
	bool			spawn_socket_full(slay_full_t *slay_full)	throw();

	/*************** tcp_client_t stuff	*******************************/	
	tcp_client_t *		m_tcp_client;	//!< to initiate the tcp connection
	bool			neoip_tcp_client_event_cb(void *userptr, tcp_client_t &cb_stcp_client
						, const tcp_event_t &tcp_event)	throw();
	bool			handle_cnx_established()			throw();
	bool			handle_recved_data(pkt_t &pkt)			throw();
	bool			handle_maysend_on()				throw();

	/*************** callback stuff	***************************************/	
	socket_itor_vapi_cb_t *	callback;
	void *			userptr;
	bool			notify_callback_err(const slay_err_t &slay_err)		throw();
	bool			notify_callback_err(const std::string &reason)		throw();
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_itor_stcp_t()							throw();
	~socket_itor_stcp_t()							throw();

	/*************** Setup function	***************************************/
	void		set_profile(const socket_profile_t &socket_profile)	throw();
	socket_err_t	setup(const socket_type_t &p_socket_type, const socket_addr_t &p_local_addr
				, const socket_addr_t &p_remote_addr
				, socket_itor_vapi_cb_t *callback, void* userptr)throw();
	socket_err_t	start()							throw();

	/*************** query function	***************************************/
	const socket_domain_t &	domain()	const throw()	{ return socket_helper_stcp_t::DOMAIN_VAR;}
	const socket_type_t &	type()		const throw()	{ return m_socket_type;		}		
	const socket_profile_t &profile()	const throw()	{ return m_socket_profile;	}
	const socket_addr_t &	local_addr()	const throw()	{ return m_local_addr;		}
	const socket_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;		}
	
	/*************** display function	*******************************/	
	std::string		to_string()	const throw();

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(socket_itor_vapi_t, socket_itor_stcp_t);\

	/*************** List of friend class	*******************************/
	friend class	socket_itor_stcp_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ITOR_STCP_HPP__ */




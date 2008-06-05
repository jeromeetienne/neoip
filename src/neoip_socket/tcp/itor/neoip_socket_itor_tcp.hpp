/*! \file
    \brief Declaration of the \ref socket_itor_tcp_t
*/


#ifndef __NEOIP_SOCKET_ITOR_TCP_HPP__
#define __NEOIP_SOCKET_ITOR_TCP_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_itor_tcp_wikidbg.hpp"
#include "neoip_socket_helper_tcp.hpp"
#include "neoip_socket_itor_vapi.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_tcp_itor_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_itor_t;
class	tcp_full_t;

/** \brief class for the \ref socket_itor_t of the TCP domain
 */
class socket_itor_tcp_t : public socket_itor_vapi_t, public tcp_itor_cb_t
			, private wikidbg_obj_t<socket_itor_tcp_t, socket_itor_tcp_wikidbg_init>
			{
private:
	socket_type_t		m_socket_type;		//!< the socket_type_t for this initiator
	socket_profile_t	m_socket_profile;	//!< the socket_profile_t for this socket
	socket_addr_t		m_local_addr;		//!< the local socket_addr_t
	socket_addr_t		m_remote_addr;		//!< the remote socket_addr_t

	/*************** tcp_itor_t stuff	*******************************/	
	tcp_itor_t *		tcp_itor;	//!< to initiate the tcp connection
	bool			neoip_tcp_itor_event_cb(void *userptr, tcp_itor_t &cb_tcp_itor
							, const tcp_event_t &tcp_event)	throw();
	bool			recv_cnx_established(tcp_full_t *tcp_full)			throw();

	/*************** callback stuff	***************************************/	
	socket_itor_vapi_cb_t *	callback;
	void *			userptr;
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_itor_tcp_t()							throw();
	~socket_itor_tcp_t()							throw();

	/*************** Setup function	***************************************/
	void		set_profile(const socket_profile_t &socket_profile)	throw();
	socket_err_t	setup(const socket_type_t &p_socket_type, const socket_addr_t &p_local_addr
				, const socket_addr_t &p_remote_addr
				, socket_itor_vapi_cb_t *callback, void* userptr)throw();
	socket_err_t	start()							throw();

	/*************** query function	***************************************/
	const socket_domain_t &	domain()	const throw()	{ return socket_helper_tcp_t::DOMAIN_VAR;}
	const socket_type_t &	type()		const throw()	{ return m_socket_type;		}		
	const socket_profile_t &profile()	const throw()	{ return m_socket_profile;	}
	const socket_addr_t &	local_addr()	const throw()	{ return m_local_addr;		}
	const socket_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;		}
	
	/*************** display function	*******************************/	
	std::string	to_string()		const throw();

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(socket_itor_vapi_t, socket_itor_tcp_t);

	/*************** List of friend class	*******************************/
	friend class	socket_itor_tcp_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ITOR_TCP_HPP__ */




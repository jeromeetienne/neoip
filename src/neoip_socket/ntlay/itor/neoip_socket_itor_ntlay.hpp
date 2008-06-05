/*! \file
    \brief Declaration of the \ref socket_itor_ntlay_t
*/


#ifndef __NEOIP_SOCKET_ITOR_NTLAY_HPP__
#define __NEOIP_SOCKET_ITOR_NTLAY_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_helper_ntlay.hpp"
#include "neoip_socket_itor_vapi.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_ntudp_client_cb.hpp"
#include "neoip_nlay_itor.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class socket_itor_t;

/** \brief class for the \ref socket_itor_t of the socket_domain_t::NTLAY
 */
class socket_itor_ntlay_t : public socket_itor_vapi_t, public ntudp_client_cb_t, public nlay_itor_cb_t
									, public timeout_cb_t {
private:
	socket_type_t		m_socket_type;		//!< the socket_type_t for this initiator
	socket_profile_t	m_socket_profile;	//!< the socket_profile_t for this socket
	socket_addr_t		m_local_addr;		//!< the local socket_addr_t
	socket_addr_t		m_remote_addr;		//!< the remote socket_addr_t
	
	/*************** itor_timeout stuff	*******************************/
	timeout_t		itor_timeout;	//!< timeout to stop itor too long to be established
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** ntudp_client_t stuff	*******************************/	
	ntudp_client_t *	ntudp_client;	//!< to initiate the udp connection
	bool			neoip_ntudp_client_event_cb(void *userptr, ntudp_client_t &cb_ntudp_client
						, const ntudp_event_t &ntudp_event)	throw();
	bool			recv_ntudp_cnx_established()				throw();
	bool			recv_ntudp_data(pkt_t &pkt)				throw();

	/*************** nlay_itor_t stuff	*******************************/	
	nlay_itor_t *		nlay_itor;	//!< to initiate the nlay connection
	bool			neoip_nlay_itor_event_cb(void *cb_userptr, nlay_itor_t &cb_nlay_itor
						, const nlay_event_t &nlay_event)	throw();
	bool			notify_cnx_established(nlay_full_t *nlay_full)		throw();

	/*************** callback stuff	***************************************/	
	socket_itor_vapi_cb_t *	callback;
	void *			userptr;
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_itor_ntlay_t()							throw();
	~socket_itor_ntlay_t()							throw();

	/*************** Setup function	***************************************/
	void		set_profile(const socket_profile_t &socket_profile)	throw();
	socket_err_t	setup(const socket_type_t &socket_type
				, const socket_addr_t &local_addr, const socket_addr_t &remote_addr
				, socket_itor_vapi_cb_t *callback,void* userptr)throw();
	socket_err_t	start()							throw();

	/*************** query function	***************************************/
	const socket_domain_t &	domain()	const throw()	{ return socket_helper_ntlay_t::DOMAIN_VAR;	}
	const socket_type_t &	type()		const throw()	{ return m_socket_type;		}		
	const socket_profile_t &profile()	const throw()	{ return m_socket_profile;	}
	const socket_addr_t &	local_addr()	const throw()	{ return m_local_addr;		}
	const socket_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;		}
	
	/*************** display function	*******************************/	
	std::string	to_string()		const throw();

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(socket_itor_vapi_t, socket_itor_ntlay_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ITOR_NTLAY_HPP__ */




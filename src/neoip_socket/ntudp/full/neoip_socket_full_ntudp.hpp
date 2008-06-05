/*! \file
    \brief Declaration of the neoip_socket_ntudp_dfull
*/


#ifndef __NEOIP_SOCKET_FULL_NTUDP_HPP__
#define __NEOIP_SOCKET_FULL_NTUDP_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_helper_ntudp.hpp"
#include "neoip_socket_full_vapi.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_ntudp_full_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_full_t;
class	socket_profile_t;

/** \brief class for the \ref socket_full_t of the UDP domain
 */
class socket_full_ntudp_t : NEOIP_COPY_CTOR_DENY, private ntudp_full_cb_t, public socket_full_vapi_t {
private:
	socket_type_t		m_socket_type;		//!< the socket_type_t for this initiator
	socket_profile_t	m_socket_profile;	//!< the socket_profile_t for this socket
	socket_addr_t		m_local_addr;		//!< the local socket_addr_t
	socket_addr_t		m_remote_addr;		//!< the remote socket_addr_t

	/*************** ntudp_full_t stuff	*******************************/
	ntudp_full_t *		ntudp_full;
	bool			neoip_ntudp_full_event_cb(void *userptr, ntudp_full_t &cb_ntudp_full
						, const ntudp_event_t &ntudp_event)	throw();

	/*************** callback stuff	***************************************/
	socket_full_vapi_cb_t *	callback;	//!< callback to notify event
	void *			userptr;	//!< userptr associated with the event callback
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

	/*************** Setup function	***************************************/
	socket_err_t	set_ctor_param(ntudp_full_t *ntudp_full
				, const socket_profile_t &socket_profile)	throw();
public:
	/*************** ctor/dtor	***************************************/
	socket_full_ntudp_t()							throw();
	~socket_full_ntudp_t()							throw();
	
	void		notify_glue_destruction()				throw();
	// function inherited from socket_full_vapi_t
	void		set_callback(socket_full_vapi_cb_t *callback, void *userptr)	throw();
	socket_err_t	start()								throw();


	void		rcvdata_maxlen(size_t recv_max_len)			throw();
	size_t		rcvdata_maxlen()					const throw();
	size_t		send(const void *data_ptr, size_t data_len) 		throw();

	/*************** various get	***************************************/
	const socket_domain_t &	domain()	const throw()	{ return socket_helper_ntudp_t::DOMAIN_VAR;}
	const socket_type_t &	type()		const throw()	{ return m_socket_type;		}		
	const socket_profile_t &profile()	const throw()	{ return m_socket_profile;	}
	const socket_addr_t &	local_addr()	const throw()	{ return m_local_addr;		}
	const socket_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;		}

	/*************** socket_mtu_vapi_t accessor	***********************/
	const socket_mtu_vapi_t*mtu_vapi()		const throw();
	socket_mtu_vapi_t *	mtu_vapi()		throw();
	bool			mtu_vapi_avail()	const throw()	{ return true;	}
	
	/*************** Display function	*******************************/
	std::string		to_string()	const throw();	

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(socket_full_vapi_t, socket_full_ntudp_t);
	
	/*************** List of friend class	*******************************/
	friend class	socket_resp_ntudp_t;
	friend class	socket_itor_ntudp_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_FULL_NTUDP_HPP__ */




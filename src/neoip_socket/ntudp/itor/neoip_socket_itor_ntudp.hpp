/*! \file
    \brief Declaration of the \ref socket_itor_ntudp_t
*/


#ifndef __NEOIP_SOCKET_ITOR_NTUDP_HPP__
#define __NEOIP_SOCKET_ITOR_NTUDP_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_helper_ntudp.hpp"
#include "neoip_socket_itor_vapi.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_ntudp_itor_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_itor_t;
class	ntudp_full_t;

/** \brief class for the \ref socket_itor_t of the NTUDP domain
 */
class socket_itor_ntudp_t : public socket_itor_vapi_t, public ntudp_itor_cb_t {
private:
	socket_type_t		m_socket_type;		//!< the socket_type_t for this initiator
	socket_profile_t	m_socket_profile;	//!< the socket_profile_t for this socket
	socket_addr_t		m_local_addr;		//!< the local socket_addr_t
	socket_addr_t		m_remote_addr;		//!< the remote socket_addr_t

	/*************** ntudp_itor_t stuff	*******************************/	
	ntudp_itor_t *		ntudp_itor;	//!< to initiate the ntudp connection
	bool			neoip_ntudp_itor_event_cb(void *userptr, ntudp_itor_t &cb_ntudp_itor
							, const ntudp_event_t &ntudp_event)	throw();
	bool			recv_cnx_established(ntudp_full_t *ntudp_full)			throw();

	/*************** callback stuff	***************************************/	
	socket_itor_vapi_cb_t *	callback;
	void *			userptr;
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_itor_ntudp_t()							throw();
	~socket_itor_ntudp_t()							throw();

	/*************** Setup function	***************************************/
	void		set_profile(const socket_profile_t &socket_profile)	throw();
	socket_err_t	setup(const socket_type_t &socket_type
				, const socket_addr_t &local_addr, const socket_addr_t &remote_addr
				, socket_itor_vapi_cb_t *callback, void* userptr)throw();
	socket_err_t	start()							throw();

	/*************** query function	***************************************/
	const socket_domain_t &	domain()	const throw()	{ return socket_helper_ntudp_t::DOMAIN_VAR;	}
	const socket_type_t &	type()		const throw()	{ return m_socket_type;		}		
	const socket_profile_t &profile()	const throw()	{ return m_socket_profile;	}
	const socket_addr_t &	local_addr()	const throw()	{ return m_local_addr;		}
	const socket_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;		}
	
	/*************** display function	*******************************/	
	std::string		to_string()	const throw();

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(socket_itor_vapi_t, socket_itor_ntudp_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ITOR_NTUDP_HPP__ */




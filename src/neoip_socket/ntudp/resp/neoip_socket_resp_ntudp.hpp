/*! \file
    \brief Declaration of the neoip_socket_ntudp_dresp
*/


#ifndef __NEOIP_SOCKET_RESP_NTUDP_HPP__
#define __NEOIP_SOCKET_RESP_NTUDP_HPP__
/* system include */
/* local include */
#include "neoip_socket_helper_ntudp.hpp"
#include "neoip_socket_resp_vapi.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_ntudp_resp_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_resp_t;
class	ntudp_full_t;

/** \brief class for the \ref socket_resp_t of the UDP domain
 */
class socket_resp_ntudp_t : public socket_resp_vapi_t, public ntudp_resp_cb_t {
private:
	socket_type_t		m_socket_type;		//!< the socket_type_t issued by this responder
	socket_profile_t	m_socket_profile;	//!< the socket_profile_t for this responder
	socket_addr_t		m_listen_addr;		//!< the socket_addr_t on which it is listening on

	/*************** ntudp_resp_t stuff	*******************************/
	ntudp_resp_t *		ntudp_resp;
	bool			neoip_ntudp_resp_event_cb(void *userptr, ntudp_resp_t &cb_ntudp_resp
							, const ntudp_event_t &ntudp_event)	throw();
	bool			recv_cnx_established(ntudp_full_t *ntudp_full)			throw();

	/*************** callback stuff	***************************************/
	socket_resp_vapi_cb_t *	callback;		//!< the callback used to notify events
	void *			userptr;		//!< the userptr associated with the callback
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_resp_ntudp_t()							throw();
	~socket_resp_ntudp_t()							throw();
	
	/*************** Setup function	***************************************/
	void		set_profile(const socket_profile_t &socket_profile)		throw();
	socket_err_t	setup(const socket_type_t &socket_type, const socket_addr_t &listen_addr
				, socket_resp_vapi_cb_t *callback, void* userptr)	throw();
	socket_err_t	start()								throw();

	/*************** query function	***************************************/
	const socket_domain_t &	domain()	const throw()	{ return socket_helper_ntudp_t::DOMAIN_VAR;	}
	const socket_type_t &	type()		const throw()	{ return m_socket_type;		}		
	const socket_profile_t &profile()	const throw()	{ return m_socket_profile;	}
	const socket_addr_t &	listen_addr()	const throw()	{ return m_listen_addr;		}

	/*************** display function	*******************************/
	std::string	to_string()					  	const throw();

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(socket_resp_vapi_t, socket_resp_ntudp_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_RESP_NTUDP_HPP__ */




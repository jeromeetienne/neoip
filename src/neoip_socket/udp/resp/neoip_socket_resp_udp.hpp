/*! \file
    \brief Declaration of the neoip_socket_udp_dresp
*/


#ifndef __NEOIP_SOCKET_RESP_UDP_HPP__
#define __NEOIP_SOCKET_RESP_UDP_HPP__
/* system include */
#include <list>
#include <iostream>
/* local include */
#include "neoip_socket_helper_udp.hpp"
#include "neoip_socket_resp_udp.hpp"
#include "neoip_socket_resp_vapi.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_udp_resp.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class socket_resp_t;
class nlay_resp_t;
class nlay_full_t;

/** \brief class for the \ref socket_resp_t of the UDP domain
 */
class socket_resp_udp_t : public socket_resp_vapi_t, public udp_resp_cb_t {
private:
	socket_type_t		m_socket_type;		//!< the socket_type_t issued by this responder
	socket_profile_t	m_socket_profile;	//!< the socket_profile_t for this responder
	socket_addr_t		m_listen_addr;		//!< the socket_addr_t on which it is listening on

	/*************** List of incoming connection	***********************/
	class			cnx_t;
	std::list<cnx_t *>	cnx_list;
	void 			cnx_dolink(cnx_t *cnx)	throw()	{ cnx_list.push_back(cnx);	}
	void 			cnx_unlink(cnx_t *cnx)	throw()	{ cnx_list.remove(cnx);		}
							
	/*************** udp_resp_t stuff	*******************************/
	udp_resp_t *		udp_resp;
	bool			neoip_inet_udp_resp_event_cb(void *userptr, udp_resp_t &cb_udp_resp
							, const udp_event_t &udp_event)		throw();
	/*************** nlay_resp_t stuff	*******************************/
	nlay_resp_t *		nlay_resp;
	
	/*************** callback stuff	***************************************/
	socket_resp_vapi_cb_t *	callback;		//!< the callback used to notify events
	void *			userptr;		//!< the userptr associated with the callback
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_resp_udp_t()							throw();
	~socket_resp_udp_t()							throw();
	
	/*************** Setup function	***************************************/
	void		set_profile(const socket_profile_t &p_socket_profile)		throw();
	socket_err_t	setup(const socket_type_t &p_socket_type, const socket_addr_t &p_listen_addr
				, socket_resp_vapi_cb_t *callback, void* userptr)	throw();
	socket_err_t	start()								throw();
		
	/*************** query function	***************************************/
	const socket_domain_t &	domain()	const throw()	{ return socket_helper_udp_t::DOMAIN_VAR;}
	const socket_type_t &	type()		const throw()	{ return m_socket_type;		}		
	const socket_profile_t &profile()	const throw()	{ return m_socket_profile;	}
	const socket_addr_t &	listen_addr()	const throw()	{ return m_listen_addr;		}

	/*************** display function	*******************************/
	std::string	to_string()					  	const throw();

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(socket_resp_vapi_t, socket_resp_udp_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_RESP_UDP_HPP__ */




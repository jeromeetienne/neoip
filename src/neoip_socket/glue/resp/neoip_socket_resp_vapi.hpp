/*! \file
    \brief Header of the \ref socket_resp_vapi_t
*/


#ifndef __NEOIP_SOCKET_RESP_VAPI_HPP__ 
#define __NEOIP_SOCKET_RESP_VAPI_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_socket_resp_vapi_cb.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_type.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_socket_type.hpp"
#include "neoip_socket_err.hpp"
#include "neoip_obj_factory.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Definition of the virtual API for the socket responder
 */
class socket_resp_vapi_t {
public:
	/*************** setup function	***************************************/
	virtual void		set_profile(const socket_profile_t &socket_profile)	throw() = 0;
	virtual socket_err_t	setup(const socket_type_t &socket_type, const socket_addr_t &listen_addr
					, socket_resp_vapi_cb_t *callback,void* userptr)throw() = 0;
	virtual socket_err_t	start()							throw() = 0;

	/*************** query function	***************************************/
	// TODO not sure about the profile() query - this consume a lot of memory and is unused
	virtual	const socket_domain_t &	domain()				const throw() = 0;
	virtual const socket_type_t &	type()					const throw() = 0;
	virtual const socket_profile_t &profile()				const throw() = 0;
	virtual const socket_addr_t &	listen_addr()				const throw() = 0;
	
	/*************** display function	*******************************/
	virtual std::string	to_string()					const throw() = 0;
	
	// declaration for the factory
	FACTORY_BASE_CLASS_DECLARATION(socket_resp_vapi_t);
	//! virtual destructor
	virtual ~socket_resp_vapi_t() {};
};

// declaration of the factory type
FACTORY_PLANT_DECLARATION(socket_resp_factory, socket_domain_t, socket_resp_vapi_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_RESP_VAPI_HPP__  */




/*! \file
    \brief Header of the \ref socket_profile_vapi_t
*/


#ifndef __NEOIP_SOCKET_PROFILE_VAPI_HPP__ 
#define __NEOIP_SOCKET_PROFILE_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_socket_err.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_obj_factory.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Definition of the virtual API for the socket_profile_t
 */
class socket_profile_vapi_t {
public:
	/*************** query function`***************************************/
	virtual socket_err_t	check()			const throw() = 0;
	virtual	socket_domain_t	get_domain()		const throw() = 0;

	// declaration for the factory
	FACTORY_BASE_CLASS_DECLARATION(socket_profile_vapi_t);
	//! virtual destructor
	virtual ~socket_profile_vapi_t() {};
};

// declaration of the factory type
FACTORY_PLANT_DECLARATION(socket_profile_factory, socket_domain_t, socket_profile_vapi_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PROFILE_VAPI_HPP__  */




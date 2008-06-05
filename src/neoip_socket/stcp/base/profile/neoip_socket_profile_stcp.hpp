/*! \file
    \brief Declaration of the neoip_socket_stcp_daddr
*/


#ifndef __NEOIP_SOCKET_PROFILE_STCP_HPP__
#define __NEOIP_SOCKET_PROFILE_STCP_HPP__
/* system include */
/* local include */
#include "neoip_socket_profile_vapi.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_namespace.hpp"

// include the types for the profile fields
#include "neoip_slay_profile.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief socket_domain_t::STCP implementation of the socket_profile_vapi_t
 */
class socket_profile_stcp_t : public socket_profile_vapi_t {
private:
public:
	/*************** ctor/dtor	***************************************/
	socket_profile_stcp_t()	throw();

	/*************** query function	***************************************/
	socket_err_t	check()		const throw();
	socket_domain_t	get_domain()	const throw()	{ return socket_domain_t::STCP;	}
	
	/*************** helper	***********************************************/
	static socket_profile_stcp_t &		from_socket(socket_profile_t &socket_profile)		throw();
	static const socket_profile_stcp_t &	from_socket(const socket_profile_t &socket_profile)	throw();

	
	/*************** #define to ease the declaration	***************/
#	define NEOIP_VAR_DIRECT(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; 	}	\
		socket_profile_stcp_t &var_name(const var_type &value)	throw()				\
					{  var_name ## _val = value; return *this;	}
#	define NEOIP_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val;	}	\
		NEOIP_VAR_DIRECT(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	NEOIP_VAR_STRUCT( slay_profile_t	, slay	);

	/*************** #undef to ease the declaration	***********************/
#	undef NEOIP_VAR_DIRECT	
#	undef NEOIP_VAR_STRUCT
	
	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(socket_profile_vapi_t, socket_profile_stcp_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PROFILE_STCP_HPP__ */




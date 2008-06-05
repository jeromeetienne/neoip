/*! \file
    \brief Header of the \ref socket_profile_t

*/


#ifndef __NEOIP_SOCKET_PROFILE_HPP__ 
#define __NEOIP_SOCKET_PROFILE_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_err.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// include the types for the profile fields
#include "neoip_delay.hpp"
#include "neoip_nlay_profile.hpp"
#include "neoip_socket_full_profile.hpp"
#include "neoip_socket_linger_profile.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_profile_vapi_t;

// list of forward declaration for accessing domain specific API
class	socket_profile_udp_t;
class	socket_profile_ntudp_t;
class	socket_profile_ntlay_t;

/** \ref class to store the socket profile
 */
class socket_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default amount of time before a socket_itor_t times out
	static const delay_t	ITOR_TIMEOUT;
private:
	socket_profile_vapi_t *	m_profile_vapi;	//!< pointer on the domain specific api
public:
	/*************** ctor/dtor	***************************************/
	socket_profile_t(const socket_domain_t &socket_domain)	throw();
	socket_profile_t()	throw();
	~socket_profile_t()	throw();

	/*************** object copy stuff	*******************************/
	socket_profile_t(const socket_profile_t &other)			throw();
	socket_profile_t &operator = (const socket_profile_t & other)	throw();

	/*************** Query function	***************************************/
	bool		is_null()	const throw()	{ return !m_profile_vapi;	}
	socket_domain_t	get_domain()	const throw();
	socket_err_t	check()		const throw();
	socket_profile_vapi_t *	profile_vapi()	const throw()	{ return m_profile_vapi;	}

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)									\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ DBG_ASSERT( !is_null() );	\
									  return var_name ## _val; }	\
		socket_profile_t &	var_name(const var_type &value)	throw()				\
						{ DBG_ASSERT( !is_null() );				\
						  var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ DBG_ASSERT( !is_null() );		\
								  return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( delay_t			, itor_timeout);
	PROFILE_VAR_SPROF( socket_linger_profile_t	, linger);
	PROFILE_VAR_SPROF( socket_full_profile_t	, full);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN	
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PROFILE_HPP__  */






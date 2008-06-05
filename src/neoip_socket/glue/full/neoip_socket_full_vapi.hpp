/*! \file
    \brief Header of the \ref socket_full_vapi_t
*/


#ifndef __NEOIP_SOCKET_FULL_VAPI_HPP__ 
#define __NEOIP_SOCKET_FULL_VAPI_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_socket_full_vapi_cb.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_socket_type.hpp"
#include "neoip_socket_err.hpp"
#include "neoip_obj_factory.hpp"
#include "neoip_object_slotid.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_rate_vapi_t;
class	socket_mtu_vapi_t;
class	socket_stream_vapi_t;
class	socket_profile_t;

/** \brief Definition of the virtual API for the socket_full_t implementations
 */
class socket_full_vapi_t : public object_slotid_t {
public:
	// set parameter function
	virtual void		set_callback(socket_full_vapi_cb_t *callback, void *userptr)throw() = 0;
	virtual socket_err_t	start()							throw() = 0;
	virtual std::string	to_string()					  const throw() = 0;

	//! called when the socket's caller destroy the object
	virtual void		notify_glue_destruction()	throw() = 0;
	
	// TODO to put all that in a socket_common_vapi_t
	// TODO not sure about the profile() query - this consume a lot of memory and is unused
	virtual const socket_domain_t &	domain()				const throw() = 0;
	virtual const socket_type_t &	type()					const throw() = 0;
	virtual const socket_profile_t &profile()				const throw() = 0;
	virtual const socket_addr_t &	local_addr()				const throw() = 0;
	virtual const socket_addr_t &	remote_addr()				const throw() = 0;

	virtual void		rcvdata_maxlen(size_t new_value)		throw() = 0;
	virtual	size_t		rcvdata_maxlen()				const throw() = 0;
	virtual size_t		send(const void *data_ptr, size_t data_len)	throw() = 0;

	/*************** handle socket_rate_vapi_t	***********************/
	virtual const socket_stream_vapi_t*stream_vapi()	const throw()	{ DBG_ASSERT(0); return NULL;	}
	virtual socket_stream_vapi_t *	stream_vapi()		throw()		{ DBG_ASSERT(0); return NULL;	}
	virtual bool			stream_vapi_avail()	const throw()	{ return false;			}

	/*************** handle socket_rate_vapi_t	***********************/
	virtual const socket_rate_vapi_t*rate_vapi()		const throw()	{ DBG_ASSERT(0); return NULL;	}
	virtual socket_rate_vapi_t *	rate_vapi()		throw()		{ DBG_ASSERT(0); return NULL;	}
	virtual bool			rate_vapi_avail()	const throw()	{ return false;			}

	/*************** handle socket_mtu_vapi_t	***********************/
	virtual const socket_mtu_vapi_t*mtu_vapi()		const throw()	{ DBG_ASSERT(0); return NULL;	}
	virtual socket_mtu_vapi_t *	mtu_vapi()		throw()		{ DBG_ASSERT(0); return NULL;	}
	virtual bool			mtu_vapi_avail()	const throw()	{ return false;			}

	// declaration for the factory
	FACTORY_BASE_CLASS_DECLARATION(socket_full_vapi_t);
	//! virtual destructor
	virtual ~socket_full_vapi_t() {};
};

// declaration of the factory type
FACTORY_PLANT_DECLARATION(socket_full_factory, socket_domain_t, socket_full_vapi_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_FULL_VAPI_HPP__  */




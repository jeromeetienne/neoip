/*! \file
    \brief Header of the socket_itor_t
*/


#ifndef __NEOIP_SOCKET_ITOR_HPP__ 
#define __NEOIP_SOCKET_ITOR_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_itor_wikidbg.hpp"
#include "neoip_socket_itor_cb.hpp"
#include "neoip_socket_itor_arg.hpp"
#include "neoip_socket_itor_vapi.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_type.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_socket_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \ref class to store the socket itor
 */
class socket_itor_t : NEOIP_COPY_CTOR_DENY, private socket_itor_vapi_cb_t
			, private wikidbg_obj_t<socket_itor_t, socket_itor_wikidbg_init> {
private:
	/*************** socket_itor_vapi_t	*******************************/
	socket_itor_vapi_t *	m_itor_vapi;	//!< virtual api pointing on the domain implementation
	bool			neoip_socket_itor_vapi_cb(void *userptr, socket_itor_vapi_t &cb_itor_vapi
						, const socket_event_t &socket_event)		throw();
	socket_itor_vapi_t *	itor_vapi()	throw()		{ DBG_ASSERT(m_itor_vapi); return m_itor_vapi;	}
	const socket_itor_vapi_t*itor_vapi()	const throw()	{ DBG_ASSERT(m_itor_vapi); return m_itor_vapi;	}

	/*************** callback stuff	***************************************/
	socket_itor_cb_t *	callback;	//!< callback to notify event
	void *			userptr;	//!< userptr associated with the event callback
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks	
public:
	/*************** ctor/dtor	***************************************/
	socket_itor_t() 	throw();
	~socket_itor_t() 	throw();

	/*************** Setup Function	***************************************/
	socket_itor_t &	set_profile(const socket_profile_t &socket_profile)		throw();
	socket_err_t	setup(const socket_domain_t &socket_domain, const socket_type_t &socket_type
					, const socket_addr_t &local_addr, const socket_addr_t &remote_addr
					, socket_itor_cb_t *callback, void *userptr)	throw(); 
	socket_err_t	start()	throw()	{ return itor_vapi()->start();	}

	/*************** Setup Helper Function	*******************************/
	socket_err_t	start(const socket_domain_t &socket_domain, const socket_type_t &socket_type
					, const socket_addr_t &local_addr, const socket_addr_t &remote_addr
					, socket_itor_cb_t *callback, void *userptr)	throw();
	socket_err_t	start(const socket_itor_arg_t &arg, socket_itor_cb_t *callback, void *userptr)	throw();
	socket_err_t	setup(const socket_itor_arg_t &arg, socket_itor_cb_t *callback, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	bool		is_null()		const throw()	{ return m_itor_vapi == NULL;	}	
	
	const socket_domain_t &	domain()	const throw()	{ return itor_vapi()->domain();		}
	const socket_type_t &	type()		const throw()	{ return itor_vapi()->type();		}
	const socket_profile_t &profile()	const throw()	{ return itor_vapi()->profile();	}
	const socket_addr_t &	local_addr()	const throw()	{ return itor_vapi()->local_addr();	}
	const socket_addr_t &	remote_addr()	const throw()	{ return itor_vapi()->remote_addr();	}
	
	/*************** Compatibility Layer	*******************************/
	socket_addr_t	get_local_addr()	const throw()	{ return local_addr();	}
	socket_addr_t	get_remote_addr()	const throw()	{ return remote_addr();	}
	socket_domain_t	get_domain()		const throw()	{ return domain();	}
	socket_type_t	get_type()		const throw()	{ return type();	}

	/*************** display function	*******************************/
	std::string	to_string()		const throw()	{ return itor_vapi()->to_string();	}
	friend std::ostream & operator << (std::ostream & os, const socket_itor_t & itor)
						throw()		{ return os << itor.to_string();	}

	/*************** List of friend function	***********************/
	friend class	socket_itor_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ITOR_HPP__  */




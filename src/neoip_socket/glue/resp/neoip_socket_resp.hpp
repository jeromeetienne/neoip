/*! \file
    \brief Header of the socket_resp_t
*/


#ifndef __NEOIP_SOCKET_RESP_HPP__ 
#define __NEOIP_SOCKET_RESP_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_resp_wikidbg.hpp"
#include "neoip_socket_resp_cb.hpp"
#include "neoip_socket_resp_vapi.hpp"
#include "neoip_socket_resp_arg.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_type.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_socket_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the socket resp
 */
class socket_resp_t : NEOIP_COPY_CTOR_DENY, private socket_resp_vapi_cb_t
			, private wikidbg_obj_t<socket_resp_t, socket_resp_wikidbg_init>
			{
private:
	/*************** socket_resp_vapi_t	*******************************/
	socket_resp_vapi_t *	m_resp_vapi;	//!< virtual api pointing on the domain implementation
	bool			neoip_socket_resp_vapi_cb(void *userptr, socket_resp_vapi_t &cb_resp_vapi
						, const socket_event_t &socket_event)		throw();
	socket_resp_vapi_t *	resp_vapi()	throw()		{ DBG_ASSERT(m_resp_vapi); return m_resp_vapi;	}
	const socket_resp_vapi_t*resp_vapi()	const throw()	{ DBG_ASSERT(m_resp_vapi); return m_resp_vapi;	}

	/*************** callback stuff	***************************************/
	socket_resp_cb_t *	callback;	//!< callback to notify event
	void *			userptr;	//!< userptr associated with the event callback
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks	
public:
	/*************** ctor/dtor	***************************************/
	socket_resp_t() 	throw();
	~socket_resp_t() 	throw();
	
	/*************** Setup Function	***************************************/
	socket_resp_t &	set_profile(const socket_profile_t &socket_profile)	throw();
	socket_err_t	setup(const socket_domain_t &socket_domain, const socket_type_t &socket_type
					, const socket_addr_t &listen_addr
					, socket_resp_cb_t *callback, void *userptr)	throw(); 
	socket_err_t	start()		throw()	{ return resp_vapi()->start();	}

	/*************** Setup Helper Function	*******************************/
	socket_err_t	start(const socket_domain_t &socket_domain, const socket_type_t &socket_type
					, const socket_addr_t &listen_addr
					, socket_resp_cb_t *callback, void *userptr)	throw();
	socket_err_t	start(const socket_resp_arg_t &arg
					, socket_resp_cb_t *callback, void *userptr)	throw();
	socket_err_t	setup(const socket_resp_arg_t &arg
					, socket_resp_cb_t *callback, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	bool		is_null()		const throw()	{ return m_resp_vapi == NULL;		}

	const socket_domain_t &	domain()	const throw()	{ return resp_vapi()->domain();		}
	const socket_type_t &	type()		const throw()	{ return resp_vapi()->type();		}
	const socket_profile_t &profile()	const throw()	{ return resp_vapi()->profile();	}
	const socket_addr_t  &	listen_addr()	const throw()	{ return resp_vapi()->listen_addr();	}
	
	/*************** Compatibility Layer	*******************************/
	socket_domain_t	get_domain()		const throw()	{ return domain();	}
	socket_type_t	get_type()		const throw()	{ return type();	}
	socket_addr_t	get_listen_addr()	const throw()	{ return listen_addr();	}

	/*************** Display Function	*******************************/
	std::string	to_string()		const throw()	{ return resp_vapi()->to_string();	}	
	friend std::ostream & operator << (std::ostream & os, const socket_resp_t & resp)
						throw()		{ return os << resp.to_string();	}

	/*************** List of friend function	***********************/
	friend class	socket_resp_wikidbg_t;			
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_RESP_HPP__  */




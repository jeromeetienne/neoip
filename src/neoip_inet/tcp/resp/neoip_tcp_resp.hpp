/*! \file
    \brief Declaration of the tcp_resp_t
    
*/


#ifndef __NEOIP_TCP_RESP_HPP__ 
#define __NEOIP_TCP_RESP_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_tcp_resp_wikidbg.hpp"
#include "neoip_tcp_resp_cb.hpp"
#include "neoip_tcp_profile.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	tcp_event_t;
class	inet_err_t;
class	tcp_resp_arg_t;

/** \brief class definition for tcp_resp_t
 */
class tcp_resp_t  : NEOIP_COPY_CTOR_DENY, public fdwatch_cb_t
				, private wikidbg_obj_t<tcp_resp_t, tcp_resp_wikidbg_init>
				{
private:
	ipport_addr_t	m_listen_addr;	//!< the address on which the responder will listen on
	tcp_profile_t	m_profile;

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	fdwatch;	//!< fdwatch to watch the socket fd
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** callback stuff	***************************************/
	tcp_resp_cb_t*	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const tcp_event_t &tcp_event)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	tcp_resp_t()			throw();
	~tcp_resp_t()			throw();

	/*************** setup function	***************************************/
	tcp_resp_t &	profile(const tcp_profile_t &profile)			throw();
	inet_err_t	start(const ipport_addr_t &m_listen_addr, tcp_resp_cb_t *callback, void *userptr)throw();
	inet_err_t	start(const tcp_resp_arg_t &resp_arg, tcp_resp_cb_t *callback, void *userptr)	throw();

	/*************** query function	***************************************/
	bool			is_null()	const throw()	{ return callback == NULL;	}
	const ipport_addr_t &	listen_addr()	const throw()	{ return m_listen_addr;		}
	const tcp_profile_t &	profile()	const throw()	{ return m_profile;		}

	/*************** compatibility layer	*******************************/
	const ipport_addr_t &	get_listen_addr()const throw()	{ return listen_addr();		}

	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const tcp_resp_t &tcp_resp ) throw()
					{ return os << tcp_resp.to_string();	}

	/*************** List of friend class	*******************************/
	friend class	tcp_resp_wikidbg_t;				
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_RESP_HPP__  */




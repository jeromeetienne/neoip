/*! \file
    \brief Declaration of the tcp_itor_t
    
*/


#ifndef __NEOIP_TCP_ITOR_HPP__ 
#define __NEOIP_TCP_ITOR_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_tcp_itor_wikidbg.hpp"
#include "neoip_tcp_itor_cb.hpp"
#include "neoip_tcp_profile.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	inet_err_t;
class	tcp_itor_arg_t;

/** \brief class definition for tcp_itor_t
 * 
 * - tcp has no actual itor, this is only a stub to keep the same arch with other socket stuff
 */
class tcp_itor_t : public fdwatch_cb_t, NEOIP_COPY_CTOR_DENY, private timeout_cb_t
				, private wikidbg_obj_t<tcp_itor_t, tcp_itor_wikidbg_init>
				{
private:
	ipport_addr_t	m_local_addr;	//!< The local address of this itor (optionall)
	ipport_addr_t	m_remote_addr;	//!< the remote address of this itor (mandatory)
	tcp_profile_t	m_profile;

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	fdwatch;	//!< fdwatch to watch the socket fd
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;	//!< triggered when the remote peer has been idle too long
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** callback stuff	***************************************/
	tcp_itor_cb_t*	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const tcp_event_t &tcp_event)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	tcp_itor_t()			throw();
	~tcp_itor_t()			throw();

	/*************** setup function	***************************************/
	tcp_itor_t &	profile(const tcp_profile_t &profile)			throw();
	tcp_itor_t &	local_addr(const ipport_addr_t &p_local_addr)		throw();
	inet_err_t	start(const ipport_addr_t &p_remote_addr, tcp_itor_cb_t *callback
							, void *userptr)	throw();
	inet_err_t	start(const tcp_itor_arg_t &itor_arg, tcp_itor_cb_t *callback
							, void *userptr)	throw();

	/*************** query function	***************************************/
	bool			is_null()	const throw()	{ return callback == NULL;	}
	const ipport_addr_t &	local_addr()	const throw()	{ return m_local_addr;		}
	const ipport_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;		}
	const tcp_profile_t &	profile()	const throw()	{ return m_profile;		}

	/*************** Compatibility layer	*******************************/
	tcp_itor_t &		set_local_addr(const ipport_addr_t &p_local_addr)
						throw()		{ return local_addr(p_local_addr);}
	const ipport_addr_t &	get_local_addr()const throw()	{ return local_addr();		}
	const ipport_addr_t &	get_remote_addr()const throw()	{ return remote_addr();		}

	/*************** Display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const tcp_itor_t &tcp_itor ) throw()
					{ return os << tcp_itor.to_string();	}

	/*************** List of friend class	*******************************/
	friend class	tcp_itor_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_ITOR_HPP__  */




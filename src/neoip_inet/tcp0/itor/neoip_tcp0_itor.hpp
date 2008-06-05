/*! \file
    \brief Declaration of the tcp_itor_t
    
*/


#ifndef __NEOIP_TCP_ITOR_HPP__ 
#define __NEOIP_TCP_ITOR_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_tcp_itor_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for tcp_itor_t
 * 
 * - tcp has no actual itor, this is only a stub to keep the same arch with other socket stuff
 */
class tcp_itor_t : public fdwatch_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	ipport_addr_t	local_addr;	//!< The local address of this itor (optionall)
	ipport_addr_t	remote_addr;	//!< the remote address of this itor (mandatory)

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	fdwatch;	//!< fdwatch to watch the socket fd
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** callback stuff	***************************************/
	tcp_itor_cb_t *	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const tcp_event_t &tcp_event)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks

public:
	/*************** ctor/dtor	***************************************/
	tcp_itor_t()			throw();
	~tcp_itor_t()			throw();

	/*************** setup function	***************************************/
	tcp_itor_t &	set_local_addr(const ipport_addr_t &local_addr)		throw();
	inet_err_t	start(const ipport_addr_t &remote_addr
				, tcp_itor_cb_t *callback, void *userptr)	throw();

	/*************** query function	***************************************/
	bool			is_null()		const throw()	{ return callback == NULL;	}
	const ipport_addr_t &	get_local_addr()	const throw()	{ return local_addr;		}
	const ipport_addr_t &	get_remote_addr()	const throw()	{ return remote_addr;		}

	/*************** Display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const tcp_itor_t &tcp_itor ) throw()
					{ return os << tcp_itor.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_ITOR_HPP__  */




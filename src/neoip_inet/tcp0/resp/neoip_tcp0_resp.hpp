/*! \file
    \brief Declaration of the tcp_resp_t
    
*/


#ifndef __NEOIP_TCP_RESP_HPP__ 
#define __NEOIP_TCP_RESP_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_tcp_resp_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_tcp_event.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for tcp_resp_t
 */
class tcp_resp_t  : NEOIP_COPY_CTOR_DENY, public fdwatch_cb_t {
private:
	ipport_addr_t	listen_addr;	//!< the address on which the responder will listen on

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	fdwatch;	//!< fdwatch to watch the socket fd
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** callback stuff	***************************************/
	tcp_resp_cb_t *	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const tcp_event_t &tcp_event)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	tcp_resp_t()			throw();
	~tcp_resp_t()			throw();

	/*************** setup function	***************************************/
	inet_err_t		start(const ipport_addr_t &listen_addr, tcp_resp_cb_t *callback
							, void *userptr)	throw();

	/*************** query function	***************************************/
	bool			is_null()		const throw()	{ return callback == NULL;	}
	const ipport_addr_t &	get_listen_addr()	const throw()	{ return listen_addr;		}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const tcp_resp_t &tcp_resp ) throw()
					{ return os << tcp_resp.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_RESP_HPP__  */




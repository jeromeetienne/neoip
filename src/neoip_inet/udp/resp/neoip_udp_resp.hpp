/*! \file
    \brief Declaration of the udp_resp_t
    
*/


#ifndef __NEOIP_UDP_RESP_HPP__ 
#define __NEOIP_UDP_RESP_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_udp_resp_cb.hpp"
#include "neoip_udp_resp_wikidbg.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_udp_event.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief class definition for udp_resp_t
 */
class udp_resp_t  : public fdwatch_cb_t, NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<udp_resp_t, udp_resp_wikidbg_init>
			{
private:
	/*************** parameter	***************************************/
	ipport_addr_t	m_listen_addr;	//!< the address on which the responder will listen on

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	fdwatch;	//!< fdwatch to watch the socket fd
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** callback	***************************************/
	udp_resp_cb_t *	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const udp_event_t &udp_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
	
public:
	/*************** ctor/dtor	***************************************/
	udp_resp_t()			throw();
	~udp_resp_t()			throw();

	/*************** setup function	***************************************/
	inet_err_t		set_callback(udp_resp_cb_t *callback, void *userptr)	throw();
	inet_err_t		set_listen_addr(const ipport_addr_t &m_listen_addr)	throw();
	inet_err_t		start()		throw();
	inet_err_t		start(const ipport_addr_t &m_listen_addr
					, udp_resp_cb_t *callback, void *userptr)	throw();

	/*************** query function	***************************************/ 
	bool			is_null()	const throw()	{ return callback == NULL;	}
	const ipport_addr_t &	listen_addr()	const throw()	{ return m_listen_addr;		}

	/*************** Compatibility layer	*******************************/
	const ipport_addr_t &	get_listen_addr()const throw()	{ return listen_addr();		}

	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const udp_resp_t &udp_resp ) throw()
						{ return os << udp_resp.to_string();	}

	/*************** List of friend function	***********************/
	friend class	udp_resp_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_RESP_HPP__  */




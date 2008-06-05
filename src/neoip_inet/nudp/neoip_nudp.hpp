/*! \file
    \brief Declaration of the nudp_t
    
*/


#ifndef __NEOIP_NUDP_HPP__ 
#define __NEOIP_NUDP_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_nudp_cb.hpp"
#include "neoip_nudp_wikidbg.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief class definition for nudp_t
 */
class nudp_t  : public fdwatch_cb_t, NEOIP_COPY_CTOR_DENY
				, private wikidbg_obj_t<nudp_t, nudp_wikidbg_init> {
private:
	ipport_addr_t	listen_addr;	//!< the address on which the responder will listen on

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	fdwatch;	//!< fdwatch to watch the socket fd
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	ssize_t		recvfromto(void *buf_ptr, int buf_len, ipport_addr_t &local_addr
						, ipport_addr_t &remote_addr)	throw();

	/*************** callback stuff	***************************************/
	nudp_cb_t *	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks	
public:

	/*************** ctor/dtor	***************************************/
	nudp_t()			throw();
	~nudp_t()			throw();

	/*************** Setup function	***************************************/
	inet_err_t		set_callback(nudp_cb_t *callback, void *userptr)	throw();
	inet_err_t		set_listen_addr(const ipport_addr_t &listen_addr)	throw();
	inet_err_t		start()		throw();
	inet_err_t		start(const ipport_addr_t &listen_addr, nudp_cb_t *callback
								, void *userptr)	throw();

	/*************** Query function	***************************************/
	bool			is_null()		const throw();
	void			nullify()		throw();
	const ipport_addr_t &	get_listen_addr()	const throw();


	/*************** multicast group subscription	***********************/
	inet_err_t	subscribe(const ip_addr_t &ip_addr)	throw();
	inet_err_t	unsubscribe(const ip_addr_t &ip_addr)	throw();

	/*************** to send data	***************************************/
	inet_err_t	send_to(const void *buf_ptr, size_t buf_len
					, const ipport_addr_t &remote_addr)	throw();
	inet_err_t	send_to(const pkt_t &pkt, const ipport_addr_t &remote_addr)	throw()
			{ return send_to(pkt.get_data(), pkt.get_len(), remote_addr);	}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const nudp_t &nudp ) throw()
				{ return os << nudp.to_string();	}
	/*************** Friend Class	***************************************/
	friend	class	nudp_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUDP_HPP__  */




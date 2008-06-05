/*! \file
    \brief Declaration of the udp_itor_t
    
*/


#ifndef __NEOIP_UDP_ITOR_HPP__ 
#define __NEOIP_UDP_ITOR_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_udp_itor_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_udp_event.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for udp_itor_t
 * 
 * - udp has no actual itor, this is only a stub to keep the same arch with other socket stuff
 */
class udp_itor_t : private zerotimer_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	udp_full_t *	udp_full;	//!< the udp_full_t created *DURING* the itor

	/*************** zerotimer	***************************************/
	zerotimer_t	zerotimer;	//!< as udp has no actual itor and as the connection
					//!< MUST NOT be reported as soon as the itor start()
					//!< the zerotimer handle this delay.
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** callback	***************************************/
	udp_itor_cb_t *	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const udp_event_t &udp_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	udp_itor_t()			throw();
	~udp_itor_t()			throw();

	/*************** null function	***************************************/
	bool		is_null()	const throw();
	void		nullify()	throw();

	/*************** Setup function	***************************************/
	inet_err_t		set_callback(udp_itor_cb_t *callback, void *userptr)	throw();
	inet_err_t		set_local_addr(const ipport_addr_t &local_addr)		throw();
	const ipport_addr_t &	get_local_addr()					const throw();
	inet_err_t		set_remote_addr(const ipport_addr_t &remote_addr)	throw();
	const ipport_addr_t &	get_remote_addr()					const throw();
	inet_err_t		start()							throw();
	inet_err_t		start(const ipport_addr_t &remote_addr
					, udp_itor_cb_t *callback, void *userptr)	throw();

	/*************** Display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const udp_itor_t &udp_itor ) throw()
				{ return os << udp_itor.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_ITOR_HPP__  */




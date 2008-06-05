/*! \file
    \brief Declaration of the udp_vresp_t
    
*/


#ifndef __NEOIP_UDP_VRESP_HPP__ 
#define __NEOIP_UDP_VRESP_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <list>
#include <map>
/* local include */
#include "neoip_udp_vresp_cb.hpp"
#include "neoip_udp_vresp_wikidbg.hpp"
#include "neoip_udp_resp_cb.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	udp_vresp_cnx_t;

/** \brief class definition for udp_vresp_t
 * 
 * - it provides an API similar to udp_resp_t but allow a feature similar to
 *   http virtual host, hence the 'v' in the udp_vresp_t name.
 *   - here the 'virtual host' is a uint8_t located at the first byte of the first
 *     packet received thru the udp_full_t
 *     - the 'virtual host' is callback pkttype in this module
 * 
 * - NOTE: some issues with latency:
 *   - when a packet is received by udp_vresp_t,it perform a zerotimer before 
 *     notifying it, thus increasing the latency of notification
 *   - moreover the udp_resp_t does the same
 *   - the purpose of those zerotimer_t is not to notify packet in the same
 *     core loop iteration than the udp_full_t notification
 *   => as a consequence, when each packet is received it wait 2 zerotimer_t before
 *      being notified... the latency and possibly the cpu overhead are significant
 *      - a possible area of big trouble is if a reliable protocol with congestion
 *        is implemented on top of it. the timer of the packet will be widely inaccurate
 *      - on the other hand, it happens only for the first packet of the udp_full_t connection
 *        So it is not that frequent.
 *   => This may or may not be an issue depending on the usage pattern of udp_vresp_t
 *      - so keep this aspect in mind while using udp_vresp_t or udp_resp_t
 */
class udp_vresp_t  : NEOIP_COPY_CTOR_DENY, private udp_resp_cb_t 
			, private wikidbg_obj_t<udp_vresp_t, udp_vresp_wikidbg_init>
			{
private:
	/*************** udp responder	***************************************/
	udp_resp_t *	udp_resp;	//!< the udp_resp_t on which to listen
	bool		neoip_inet_udp_resp_event_cb(void *userptr, udp_resp_t &cb_udp_resp
						, const udp_event_t &udp_event)		throw();


	/*************** registered callback	*******************************/
	class					reg_cb_t;
	typedef std::map<uint8_t, reg_cb_t>	reg_cb_db_t;
	reg_cb_db_t			reg_cb_db;
	reg_cb_t			reg_cb_get(uint8_t pkttype)	const throw();
	
	/*************** List of incoming connection	***********************/
	std::list<udp_vresp_cnx_t *>		cnx_db;
	void cnx_link(udp_vresp_cnx_t *cnx)	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(udp_vresp_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}

public:
	/*************** ctor/dtor	***************************************/
	udp_vresp_t()		throw();
	~udp_vresp_t()		throw();

	/*************** Setup Function	***************************************/
	inet_err_t		set_listen_addr(const ipport_addr_t &listen_addr)	throw();
	inet_err_t		start()							throw();
	// helper function for start()
	inet_err_t		start(const ipport_addr_t &listen_addr)	throw();

	/*************** Query function	***************************************/
	bool			is_null()		const throw()	{ return udp_resp == NULL;	}
	const ipport_addr_t &	listen_addr()		const throw();

	/*************** Compatibility layer	*******************************/
	const ipport_addr_t &	get_listen_addr()	const throw()	{ return listen_addr();	}

	/*************** add/delete callback	*******************************/
	void	register_callback(uint8_t pkttype, udp_vresp_cb_t *callback, void *userptr)	throw();
	void	unregister_callback(uint8_t pkttype)						throw();

	/*************** display function	*******************************/
	std::string	to_string()			const throw();
	friend	std::ostream & operator << (std::ostream & os, const udp_vresp_t &udp_resp ) throw()
							{ return os << udp_resp.to_string();	}

	/*************** List of friend function	***********************/
	friend class	udp_vresp_cnx_t;
	friend class	udp_vresp_wikidbg_t;
};

/** \brief Type to store the registed pkttype in the udp_vresp_t
 */
class udp_vresp_t::reg_cb_t : NEOIP_COPY_CTOR_ALLOW {
private:
	udp_vresp_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	uint8_t		pkttype;	//!< the pkttype for which this reg_cb_t
	// TODO is this pkttype used ? i dont think so :) so remove it
public:
	reg_cb_t()	throw() : callback(NULL)	{}
	reg_cb_t(uint8_t pkttype, udp_vresp_cb_t *callback, void *userptr)	throw() 
			: callback(callback), userptr(userptr), pkttype(pkttype)	{}
	bool	is_null()	const throw()	{ return callback == NULL;	}
	
	udp_vresp_cb_t *get_callback()	throw()	{ return callback;	}
	void *		get_userptr()	throw()	{ return userptr;	}

	/*************** List of friend function	***********************/
	friend class	udp_vresp_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_VRESP_HPP__  */




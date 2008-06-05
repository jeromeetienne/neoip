/*! \file
    \brief Header of the \ref kad_rpclistener_t class
    
*/


#ifndef __NEOIP_KAD_RPCLISTENER_HPP__ 
#define __NEOIP_KAD_RPCLISTENER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_zerotimer.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	kad_rpcresp_t;
class	kad_rpclistener_id_t;
class	kad_rpcpeer_t;
class	kad_rpcpeer_id_t;
class	udp_vresp_t;
class	kad_listener_t;
class	kad_err_t;
class	xmlrpc_parse_t;
class	httpd_request_t;

/** \brief A responder for xmlrpc
 */
class kad_rpclistener_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t, private timeout_cb_t {
private:
	kad_rpcresp_t *		m_rpcresp;	//!< backpointer to the kad_rpcresp_t
	udp_vresp_t *		m_udp_vresp;	//!< the core udp_vresp_t listening on a ipport_addr_t
	kad_listener_t *	m_kad_listener;	//!< the kad_listener for this ntudp_peer_t 
	httpd_request_t *	httpd_request;	//!< store httpd_request_t for async reply notification

	/*************** zerotimer_t	***************************************/
	zerotimer_t		reply_zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** idle_timeout stuff	*******************************/
	delay_t			idle_maxdelay;	//!< the maximum amount of time this object may remain idle
	timeout_t		idle_timeout;
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** kad_rpcpeer_t	***************************************/
	typedef std::list<kad_rpcpeer_t *>	rpcpeer_db_t;
	rpcpeer_db_t				m_rpcpeer_db;
	void rpcpeer_dolink(kad_rpcpeer_t *kad_rpcpeer)	throw()	{ idle_refresh(); m_rpcpeer_db.push_back(kad_rpcpeer);	}
	void rpcpeer_unlink(kad_rpcpeer_t *kad_rpcpeer)	throw()	{ idle_refresh(); m_rpcpeer_db.remove(kad_rpcpeer);	}
	rpcpeer_db_t &	rpcpeer_db()			throw()	{ return m_rpcpeer_db;			}
public:
	/*************** ctor/dtor	***************************************/
	kad_rpclistener_t()	throw();
	~kad_rpclistener_t()	throw();
	
	/*************** Setup function	***************************************/
	kad_err_t	start(kad_rpcresp_t *m_rpcresp, const httpd_request_t &recved_request
							, const std::string &method_name	
							, xmlrpc_parse_t &xmlrpc_parse)	throw();

	/*************** query function	***************************************/
	udp_vresp_t *		udp_vresp()	const throw()	{ return m_udp_vresp;	}
	kad_listener_t *	kad_listener()	const throw()	{ return m_kad_listener;}
	kad_rpcresp_t *		rpcresp()	const throw()	{ return m_rpcresp;	}
	kad_rpclistener_id_t	rpclistener_id()const throw();
	kad_rpcpeer_t *		rpcpeer_from_id(const kad_rpcpeer_id_t &rpcpeer_id)	const throw();

	/*************** Action function	*******************************/
	void			idle_refresh()	throw();
	
	/*************** List of friend function	***********************/
	friend class	kad_rpclistener_id_t;
	friend class	kad_rpcpeer_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RPCLISTENER_HPP__  */




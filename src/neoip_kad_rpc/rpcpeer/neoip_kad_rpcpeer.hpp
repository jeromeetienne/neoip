/*! \file
    \brief Header of the \ref kad_rpcpeer_t class
    
*/


#ifndef __NEOIP_KAD_RPCPEER_HPP__ 
#define __NEOIP_KAD_RPCPEER_HPP__ 
/* system include */
/* local include */
#include "neoip_zerotimer.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	kad_rpcresp_t;
class	kad_rpcpeer_id_t;
class	kad_rpclistener_t;
class	kad_peer_t;
class	kad_err_t;
class	xmlrpc_parse_t;
class	httpd_request_t;
class	kad_rpcclosestnode_t;
class	kad_rpcdelete_t;
class	kad_rpcquery_some_t;
class	kad_rpcquery_t;
class	kad_rpcstore_t;

/** \brief A responder for xmlrpc
 */
class kad_rpcpeer_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t, private timeout_cb_t {
private:
	kad_rpclistener_t *	m_rpclistener;	//!< backpointer to the kad_rpclistener_t
	kad_peer_t *		m_kad_peer;
	httpd_request_t *	httpd_request;	//!< store httpd_request_t for async reply notification

	/*************** zerotimer_t	***************************************/
	zerotimer_t		reply_zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** idle_timeout stuff	*******************************/
	delay_t			idle_maxdelay;	//!< the maximum amount of time this object may remain idle
	timeout_t		idle_timeout;
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** kad_rpcstore_t	***************************************/
#define NEOIP_KADRPC_CMD(name)												\
	typedef std::list<kad_##name##_t *>	name##_db_t;								\
	name##_db_t &				name##_db()	throw()	{ return m_##name##_db;		}		\
	name##_db_t				m_##name##_db;								\
	void name##_dolink(kad_##name##_t *value) throw() { idle_refresh(); m_##name##_db.push_back(value);	}	\
	void name##_unlink(kad_##name##_t *value) throw() { idle_refresh(); m_##name##_db.remove(value);	}

	NEOIP_KADRPC_CMD(rpcclosestnode)
	NEOIP_KADRPC_CMD(rpcdelete)
	NEOIP_KADRPC_CMD(rpcquery_some)
	NEOIP_KADRPC_CMD(rpcquery)
	NEOIP_KADRPC_CMD(rpcstore)

#undef NEOIP_KADRPC_CMD

public:
	/*************** ctor/dtor	***************************************/
	kad_rpcpeer_t()		throw();
	~kad_rpcpeer_t()	throw();
	
	/*************** Setup function	***************************************/
	kad_err_t		start(kad_rpcresp_t *kad_rpcresp, const httpd_request_t &recved_request
							, const std::string &method_name	
							, xmlrpc_parse_t &xmlrpc_parse)		throw();
	
	/*************** Query function	***************************************/	
	kad_peer_t *		kad_peer()	const throw()	{ return m_kad_peer;	}
	kad_rpclistener_t *	rpclistener()	const throw()	{ return m_rpclistener;	}
	kad_rpcpeer_id_t	rpcpeer_id()	const throw();

	/*************** Action function	*******************************/
	void			idle_refresh()	throw();

	/*************** List of friend class	*******************************/
	friend class	kad_rpcclosestnode_t;
	friend class	kad_rpcdelete_t;
	friend class	kad_rpcquery_some_t;
	friend class	kad_rpcquery_t;
	friend class	kad_rpcstore_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RPCPEER_HPP__  */




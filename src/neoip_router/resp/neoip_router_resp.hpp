

#ifndef __NEOIP_ROUTER_RESP_HPP__ 
#define __NEOIP_ROUTER_RESP_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_router_resp_wikidbg.hpp"
#include "neoip_router_err.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_resp_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	router_peer_t;
class	router_resp_cnx_t;

/** \brief Class which implement neoip_router
 */
class router_resp_t : NEOIP_COPY_CTOR_DENY, private socket_resp_cb_t
				, private wikidbg_obj_t<router_resp_t, router_resp_wikidbg_init>
				{
private:
	router_peer_t *	router_peer;		//!< backpointer to the router_peer_t
	socket_addr_t	listen_addr;		//!< the socket_addr_t on which the router_resp_t listens

	/*************** socket_resp_t	***************************************/
	socket_resp_t *	socket_resp;
	bool		neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
							, const socket_event_t &socket_event)	throw();

	/*************** List of router_resp_cnx_t	***************************************/
	std::list<router_resp_cnx_t *>		cnx_db;
	void cnx_link(router_resp_cnx_t *cnx)	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(router_resp_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}
public:
	/*************** ctor/dtor	***************************************/
	router_resp_t(router_peer_t *router_peer)	throw();
	~router_resp_t()				throw();

	/*************** Setup Function	***************************************/
	router_err_t	start(const socket_addr_t &listen_addr)	throw();
	
	/*************** Query Function	***************************************/
	const socket_addr_t &	get_listen_addr() const throw()	{ return listen_addr;	}
	
	/*************** Friend Class	***************************************/
	friend class	router_resp_wikidbg_t;	
	friend class	router_resp_cnx_t;
	friend class	router_resp_cnx_wikidbg_t;	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_RESP_HPP__  */




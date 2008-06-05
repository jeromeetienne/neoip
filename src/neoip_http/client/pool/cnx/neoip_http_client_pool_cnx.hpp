/*! \file
    \brief Header of the http_client_pool_cnx_t
    
*/


#ifndef __NEOIP_HTTP_CLIENT_POOL_CNX_HPP__ 
#define __NEOIP_HTTP_CLIENT_POOL_CNX_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_http_client_pool_cnx_wikidbg.hpp"
#include "neoip_http_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_client_pool_t;

/** \brief class definition for http_client_pool_cnx_t
 * 
 * - it is a single socket_full_t
 */
class http_client_pool_cnx_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t, public timeout_cb_t
			, private wikidbg_obj_t<http_client_pool_cnx_t, http_client_pool_cnx_wikidbg_init>
			{
private:
	http_client_pool_t *	client_pool;	//!< backpointer to the linked http_client_pool_t
	http_uri_t		current_uri;	//!< the current http_uri_t for this socket_full_t
	std::set<http_uri_t>	handled_uri_db;	//!< the set of http_uri_t handled by this cnx

	/*************** Internal function	*******************************/
	bool		autodelete()		throw();

	/*************** expire_timeout stuff	*******************************/
	timeout_t	expire_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** socket_full_t	***************************************/
	socket_full_t *	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	http_client_pool_cnx_t() 		throw();
	~http_client_pool_cnx_t()		throw();
	
	/*************** setup function	***************************************/
	http_err_t	start(http_client_pool_t *client_pool, socket_full_t *socket_full
					, const http_uri_t &current_uri
					, const std::set<http_uri_t> &handled_uri_db)	throw();
	
	/*************** Query function	***************************************/
	bool				contain(const http_uri_t &http_uri)	const throw();
	const http_uri_t &		get_current_uri()	const throw()	{ return current_uri;	}
	const std::set<http_uri_t> &	get_handled_uri_db()	const throw()	{ return handled_uri_db;}

	/*************** Action function	*******************************/
	socket_full_t *			steal_full_and_dtor()	throw();

	/*************** List of friend class	*******************************/
	friend class	http_client_pool_cnx_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_CLIENT_POOL_CNX_HPP__  */




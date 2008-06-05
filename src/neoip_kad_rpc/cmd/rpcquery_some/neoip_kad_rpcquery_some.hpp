/*! \file
    \brief Header of the \ref kad_rpcquery_some_t class
    
*/


#ifndef __NEOIP_KAD_RPCQUERY_SOME_HPP__ 
#define __NEOIP_KAD_RPCQUERY_SOME_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_kad_query_some_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	kad_rpcresp_t;
class	kad_rpcpeer_t;
class	kad_err_t;
class	xmlrpc_parse_t;
class	httpd_request_t;

/** \brief A responder for xmlrpc
 */
class kad_rpcquery_some_t : NEOIP_COPY_CTOR_DENY, private kad_query_some_cb_t {
private:
	kad_rpcpeer_t *		m_rpcpeer;	//!< backpointer to the kad_rpclistener_t
	httpd_request_t *	httpd_request;	//!< query_some httpd_request_t for async reply notification

	/*************** kad_query_some_t	***************************************/
	kad_query_some_t *	m_kad_query_some;	//!< the kad_query_some_t for the peer record
	bool			neoip_kad_query_some_cb(void *cb_userptr, kad_query_some_t &cb_kad_query_some
						, const kad_event_t &cb_kad_event) throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_rpcquery_some_t()	throw();
	~kad_rpcquery_some_t()	throw();
	
	/*************** Setup function	***************************************/
	kad_err_t		start(kad_rpcresp_t *kad_rpcresp, const httpd_request_t &recved_request
							, const std::string &method_name
							, xmlrpc_parse_t &xmlrpc_parse)		throw();
	
	/*************** Query function	***************************************/
	kad_rpcpeer_t *		rpcpeer()	const throw()	{ return m_rpcpeer;		}
	kad_query_some_t *	kad_query_some()const throw()	{ return m_kad_query_some;	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RPCQUERY_SOME_HPP__  */




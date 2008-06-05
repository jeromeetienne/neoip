/*! \file
    \brief Header of the \ref kad_rpcstore_t class
    
*/


#ifndef __NEOIP_KAD_RPCSTORE_HPP__ 
#define __NEOIP_KAD_RPCSTORE_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_kad_store_cb.hpp"
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
class kad_rpcstore_t : NEOIP_COPY_CTOR_DENY, private kad_store_cb_t {
private:
	kad_rpcpeer_t *		m_rpcpeer;	//!< backpointer to the kad_rpclistener_t
	httpd_request_t *	httpd_request;	//!< store httpd_request_t for async reply notification

	/*************** kad_store_t	***************************************/
	kad_store_t *		m_kad_store;	//!< the kad_store_t for the peer record
	bool			neoip_kad_store_cb(void *cb_userptr, kad_store_t &cb_kad_store
						, const kad_event_t &cb_kad_event) throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_rpcstore_t()	throw();
	~kad_rpcstore_t()	throw();
	
	/*************** Setup function	***************************************/
	kad_err_t		start(kad_rpcresp_t *kad_rpcresp, const httpd_request_t &recved_request
							, const std::string &method_name
							, xmlrpc_parse_t &xmlrpc_parse)		throw();
	
	/*************** Query function	***************************************/	
	kad_rpcpeer_t *		rpcpeer()	const throw()	{ return m_rpcpeer;	}
	kad_store_t *		kad_store()	const throw()	{ return m_kad_store;	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RPCSTORE_HPP__  */




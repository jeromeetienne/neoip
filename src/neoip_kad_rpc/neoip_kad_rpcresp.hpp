/*! \file
    \brief Header of the \ref kad_rpcresp_t class
    
*/


#ifndef __NEOIP_KAD_RPCRESP_HPP__ 
#define __NEOIP_KAD_RPCRESP_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	kad_err_t;
class	kad_rpclistener_t;
class	kad_rpclistener_id_t;
class	kad_rpcpeer_t;
class	kad_rpcpeer_id_t;

/** \brief A responder for xmlrpc
 */
class kad_rpcresp_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t {
private:
	std::string 	url_path;

	/*************** old httpd callback	*******************************/
	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();

	/*************** kad_rpclistener_t	*******************************/
	typedef std::list<kad_rpclistener_t *>	rpclistener_db_t;
	rpclistener_db_t	m_rpclistener_db;
	rpclistener_db_t &	rpclistener_db()	throw()	{ return m_rpclistener_db;	}
	void rpclistener_dolink(kad_rpclistener_t *rpclistener)throw()	{ rpclistener_db().push_back(rpclistener);}
	void rpclistener_unlink(kad_rpclistener_t *rpclistener)throw()	{ rpclistener_db().remove(rpclistener);	}
public:
	/*************** ctor/dtor	***************************************/
	kad_rpcresp_t()		throw();
	~kad_rpcresp_t()	throw();

	/*************** Setup function	***************************************/
	kad_err_t	start(const std::string &url_path)	throw();

	/*************** Query function	***************************************/
	kad_rpclistener_t *	rpclistener_from_id(const kad_rpclistener_id_t &rpclistener_id) const throw();
	kad_rpcpeer_t *		rpcpeer_from_id(const kad_rpcpeer_id_t &rpcpeer_id)		const throw();

	/*************** Action function	*******************************/
	void		notify_httpd_reply(const httpd_request_t &httpd_request
					, const httpd_err_t &httpd_err)		throw();

	/*************** List of friend function	***********************/
	friend class	kad_rpclistener_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RPCRESP_HPP__  */




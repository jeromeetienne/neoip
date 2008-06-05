/*! \file
    \brief Header of the \ref httpd_t class
    
*/


#ifndef __NEOIP_HTTPD_HPP__ 
#define __NEOIP_HTTPD_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <map>
#include <list>
/* local include */
#include "neoip_httpd_handler.hpp"
#include "neoip_httpd_request.hpp"
#include "neoip_httpd_err.hpp"
#include "neoip_tcp_resp.hpp"
#include "neoip_tcp_full_cb.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief define a handler for a specific http path
 */
class httpd_t : NEOIP_COPY_CTOR_DENY, private tcp_resp_cb_t  {
private:
	std::map<std::string, httpd_handler_t>	handler_db;
	slotpool_t	cnx_slotpool;	//!< slotpool to allocate slot_id for each cnx

	// handler function
	httpd_handler_t*handler_find( const std::string &path )		throw();	

	/*************** tcp_resp_t	***************************************/
	tcp_resp_t *	tcp_resp;
	bool		neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
					, const tcp_event_t &tcp_event)		throw();

	/*************** cnx_t		***************************************/
	class				cnx_t;
	std::list<cnx_t *>		cnx_db;
	void cnx_dolink(cnx_t *cnx)	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}	
public:
	/*************** ctor/dtor	***************************************/
	httpd_t()					throw();
	~httpd_t()					throw();

	/*************** setup function	***************************************/
	bool	start(const ipport_addr_t &listen_addr)	throw();

	/*************** Query function	***************************************/
	ipport_addr_t	get_listen_addr()	const throw()	{ return tcp_resp->get_listen_addr();	}

	// handler function
	void handler_add(const httpd_handler_t &handler)		throw();
	void handler_del(const httpd_handler_t &handler)		throw();
	
	// helper function for handler
	void 	handler_add(const std::string &path, httpd_handler_cb_t *callback, void *userptr
			, httpd_handler_flag_t handler_flag = httpd_handler_t::FLAG_DFL)	throw()
			{ handler_add(httpd_handler_t(path, callback, userptr, handler_flag));	}
	void	handler_del(const std::string &path, httpd_handler_cb_t *callback, void *userptr
			, httpd_handler_flag_t handler_flag = httpd_handler_t::FLAG_DFL)	throw()
			{ handler_del(httpd_handler_t(path, callback, userptr));	}

	void	notify_delayed_reply(const httpd_request_t &request, const httpd_err_t &httpd_err)throw();

	/*************** list of friend class	*******************************/
	friend	class lib_httpd_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTPD_HPP__  */




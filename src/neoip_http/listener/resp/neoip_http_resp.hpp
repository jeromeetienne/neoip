/*! \file
    \brief Header of the \ref http_resp_t
    
*/


#ifndef __NEOIP_HTTP_RESP_HPP__ 
#define __NEOIP_HTTP_RESP_HPP__ 
/* system include */
/* local include */
#include "neoip_http_resp_wikidbg.hpp"
#include "neoip_http_resp_cb.hpp"
#include "neoip_http_resp_mode.hpp"
#include "neoip_http_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_method.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_listener_t;
class	http_reqhd_t;
class	socket_full_t;

/** \brief Handle the responder for a given uri/method attached to a given listener
 */
class http_resp_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<http_resp_t, http_resp_wikidbg_init> {
private:
	http_listener_t *	m_http_listener;	//!< backpointer to the attached http_listener_t
	http_uri_t		m_listen_uri;	//!< the uri, this responder is listening on
	http_method_t		m_listen_method;	//!< the method handled by this responder
	http_resp_mode_t	m_resp_mode;	//!< the http_resp_mode_t for this http_resp_t

	/*************** callback stuff	***************************************/
	http_resp_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const http_reqhd_t &http_reqhd, socket_full_t *socket_full
							, const bytearray_t &recved_data)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	http_resp_t()		throw();
	~http_resp_t()		throw();

	/*************** Setup function	***************************************/
	http_err_t	start(http_listener_t *m_http_listener, const http_uri_t &m_listen_uri
				, const http_method_t &m_listen_method
				, const http_resp_mode_t &m_resp_mode
				, http_resp_cb_t *callback, void *userptr)	throw();

	/*************** Query function	***************************************/
	const http_uri_t &	listen_uri()	const throw()	{ return m_listen_uri;	}
	const http_method_t &	listen_method()	const throw()	{ return m_listen_method;}
	const http_resp_mode_t &resp_mode()	const throw()	{ return m_resp_mode;	}
	bool			may_handle(const http_reqhd_t &http_reqhd)	const throw();

	/*************** Compatibility layer	*******************************/
	const http_uri_t &	get_listen_uri()	const throw() { return listen_uri();	}
	const http_method_t &	get_listen_method()	const throw() { return listen_method();	}
	const http_resp_mode_t &get_resp_mode()		const throw() { return resp_mode();	}

	
	/*************** Action function	*******************************/
	void		notify_new_cnx(const http_reqhd_t &http_reqhd, socket_full_t *socket_full
							, const bytearray_t &recved_data) throw();

	/*************** list of friend class	*******************************/
	friend class	http_resp_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_RESP_HPP__  */











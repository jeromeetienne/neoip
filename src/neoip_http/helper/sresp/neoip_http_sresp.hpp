/*! \file
    \brief Header of the \ref http_sresp_t
    
*/


#ifndef __NEOIP_HTTP_SRESP_HPP__ 
#define __NEOIP_HTTP_SRESP_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_http_sresp_wikidbg.hpp"
#include "neoip_http_sresp_profile.hpp"
#include "neoip_http_sresp_cb.hpp"
#include "neoip_http_resp_cb.hpp"
#include "neoip_http_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_sresp_cnx_t;
class	http_listener_t;
class	http_uri_t;
class	http_method_t;
class	http_resp_mode_t;

/** \brief Handle a simple synchronous http responder
 */
class http_sresp_t : NEOIP_COPY_CTOR_DENY, private http_resp_cb_t
			, private wikidbg_obj_t<http_sresp_t, http_sresp_wikidbg_init> {
private:
	http_sresp_profile_t	m_profile;	//!< the profile used by this http_sresp_t
	/*************** http_resp_t	***************************************/
	http_resp_t *	m_http_resp;
	bool		neoip_http_resp_cb(void *cb_userptr, http_resp_t &cb_http_resp
					, const http_reqhd_t &http_reqhd, socket_full_t *socket_full
					, const bytearray_t &recved_data)	throw();
	/*************** store the http_sresp_cnx_t	***********************/
	std::list<http_sresp_cnx_t *>	cnx_db;
	void cnx_dolink(http_sresp_cnx_t *cnx) 	throw()	{ cnx_db.push_back(cnx);}
	void cnx_unlink(http_sresp_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);	}

	/*************** callback stuff	***************************************/
	http_sresp_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(http_sresp_ctx_t &sresp_ctx)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	http_sresp_t()		throw();
	~http_sresp_t()		throw();

	/*************** Setup function	***************************************/
	http_sresp_t &	profile(const http_sresp_profile_t &profile)		throw();
	http_err_t	start(http_listener_t *http_listener, const http_uri_t &listen_uri
				, const http_method_t &listen_method
				, const http_resp_mode_t &resp_mode
				, http_sresp_cb_t *callback, void *userptr)		throw();

	/*************** Query function	***************************************/
	const http_sresp_profile_t &	profile()	const throw()	{ return m_profile;	}
	const http_uri_t &		listen_uri()	const throw();
	
	/*************** List of friend class	*******************************/
	friend class	http_sresp_wikidbg_t;
	friend class	http_sresp_cnx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SRESP_HPP__ */











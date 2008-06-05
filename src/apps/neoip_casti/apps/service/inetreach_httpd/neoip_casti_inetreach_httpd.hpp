/*! \file
    \brief Header of the \ref casti_inetreach_httpd_t
    
*/


#ifndef __NEOIP_CASTI_INETREACH_HTTPD_HPP__ 
#define __NEOIP_CASTI_INETREACH_HTTPD_HPP__ 
/* system include */
/* local include */
#include "neoip_casti_inetreach_httpd_wikidbg.hpp"
#include "neoip_casti_inetreach_httpd_cb.hpp"
#include "neoip_xmlrpc_resp_cb.hpp"
#include "neoip_ndiag_aview_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_listener_t;

/** \brief Handle the control via webpage
 */
class casti_inetreach_httpd_t : NEOIP_COPY_CTOR_DENY, private ndiag_aview_cb_t
			, private wikidbg_obj_t<casti_inetreach_httpd_t, casti_inetreach_httpd_wikidbg_init> {
private:
	http_listener_t *	m_http_listener;	//!< the http_listener_t to receive external http

	/*************** ndiag_aview_t	***************************************/
	ndiag_aview_t *	m_ndiag_aview;
	bool 		neoip_ndiag_aview_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
					, const ipport_addr_t &new_ipport_pview)	throw();

	/*************** callback stuff	***************************************/
	casti_inetreach_httpd_cb_t *callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const ipport_addr_t &new_listen_pview)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	casti_inetreach_httpd_t()		throw();
	~casti_inetreach_httpd_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(casti_inetreach_httpd_cb_t *callback, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	http_listener_t *	http_listener()	const throw()	{ return m_http_listener;	}
	const ipport_addr_t &	listen_ipport_lview()	const throw();
	const ipport_addr_t &	listen_ipport_pview()	const throw();
	
	/*************** List of friend class	*******************************/
	friend class	casti_inetreach_httpd_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_INETREACH_HTTPD_HPP__  */











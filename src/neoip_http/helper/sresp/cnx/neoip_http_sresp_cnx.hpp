/*! \file
    \brief Header of the http_sresp_cnx_t
    
*/


#ifndef __NEOIP_HTTP_SRESP_CNX_HPP__ 
#define __NEOIP_HTTP_SRESP_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_http_sresp_cnx_wikidbg.hpp"
#include "neoip_http_sresp_cnx_profile.hpp"
#include "neoip_http_sresp_cnx.hpp"
#include "neoip_http_err.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_sresp_t;
class	http_sresp_cnx_t;
class	http_reqhd_t;
class	http_err_t;
class	pkt_t;

/** \brief class definition for http_sresp_cnx_t
 * 
 */
class http_sresp_cnx_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t
		, public timeout_cb_t, private zerotimer_cb_t
		, private wikidbg_obj_t<http_sresp_cnx_t, http_sresp_cnx_wikidbg_init> {				
private:
	http_sresp_t *	m_http_sresp;	//!< backpointer to the linked http_sresp_t
	http_reqhd_t	m_http_reqhd;
	
	bytearray_t	m_recved_data;
	bytearray_t	data2deliver;
	
	/*************** internal function	*******************************/
	bool		autodelete(const std::string &reason = "")	throw();
	bool		notify_ctx()		throw();
	void		try_fill_xmitbuf()	throw();

	/*************** zerotimer	***************************************/
	zerotimer_t	init_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw();

	/*************** expire_timeout stuff	*******************************/
	timeout_t	expire_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** socket_full_t	***************************************/
	socket_full_t *	m_socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw();
	bool		handle_recved_data(const pkt_t &pkt)				throw();
public:
	/*************** ctor/dtor	***************************************/
	http_sresp_cnx_t() 	throw();
	~http_sresp_cnx_t()	throw();
	
	/*************** setup function	***************************************/
	http_err_t	start(http_sresp_t *http_sresp, const http_reqhd_t &http_reqhd
					, socket_full_t *socket_full
					, const bytearray_t &recved_data)	throw();

	/*************** query function	***************************************/
	const http_sresp_cnx_profile_t &	profile()	const throw();

	/*************** List of friend class	*******************************/
	friend class	http_sresp_cnx_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SRESP_CNX_HPP__  */




/*! \file
    \brief Header of the http_client_t
    
*/


#ifndef __NEOIP_HTTP_CLIENT_HPP__ 
#define __NEOIP_HTTP_CLIENT_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_http_client_wikidbg.hpp"
#include "neoip_http_client_profile.hpp"
#include "neoip_http_client_cb.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_http_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_socket_itor_cb.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_host2ip_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


// list of forward declaration
class	http_client_pool_t;
class	http_client_pool_cnx_t;
class	socket_itor_arg_t;
class	pkt_t;

/** \brief class definition for http_client
 */
class http_client_t : NEOIP_COPY_CTOR_DENY, private socket_itor_cb_t, private socket_full_cb_t
			, public timeout_cb_t, private host2ip_cb_t
			, private wikidbg_obj_t<http_client_t, http_client_wikidbg_init, socket_full_cb_t, timeout_cb_t>
			{
private:
	http_client_profile_t	profile;	//!< the profile used by this http_client_t
	http_reqhd_t		http_reqhd;	//!< the current http_reqhd_t to query
	socket_itor_arg_t *	m_orig_itor_arg;//!< copy of the socket_itor_arg_t provided by the caller

	std::set<http_uri_t>	handled_uri_db;	//!< a set of all the http_uri which has been attempted.
						//!< it is made to avoid infinite redirect loop
	http_client_pool_t *	client_pool;	//!< a pointer on a http_client_pool_t (not owned by 
						//!< this object). may be NULL if none is set

	bytearray_t		recved_data;	//!< contains the data read from the connection

	/*************** data2post stuff	*******************************/
	size_t			data2post_off;	//!< offset within the data2post - IIF http_method_t::POST
	void			send_data2post()	throw();

	/*************** Internal function	*******************************/
	http_err_t		launch_http_uri()					throw();
	http_err_t		launch_socket_client(const ip_addr_t &ip_addr)		throw();
	http_err_t		launch_by_pool_cnx(http_client_pool_cnx_t *pool_cnx)	throw();
	
	/*************** expire_timeout stuff	*******************************/
	timeout_t		expire_timeout;
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** host2ip_t	***************************************/
	host2ip_t *		host2ip;
	bool			neoip_host2ip_cb(void *cb_userptr, host2ip_vapi_t &cb_host2ip_vapi
						, const inet_err_t &inet_err
						, const std::vector<ip_addr_t> &result_arr)	throw();
	bool			may_host2ip()					const throw();
	/*************** socket_itor_t	***************************************/
	socket_itor_t *		socket_itor;	//!< to initiate the connection
	bool			neoip_socket_itor_event_cb(void *userptr, socket_itor_t &cb_socket_itor
							, const socket_event_t &socket_event)	throw();
	socket_err_t		handle_cnx_established(socket_full_t *new_socket_full)		throw();

	/*************** socket_full_t	***************************************/
	socket_full_t *		socket_full;	//!< the socket_full_t struct
	bool			neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw();
	bool			handle_recved_data(pkt_t &pkt)					throw();
	bool			handle_maysend_on()						throw();
	bool			handle_http_redirect(const http_rephd_t &http_rephd)		throw();

	/*************** callback stuff	***************************************/
	http_client_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback_failed(const http_err_t &http_err)	throw();
	bool			notify_callback_succeed(const http_rephd_t &http_rephd
						, socket_full_t *socket_full
						, const bytearray_t &recved_data)	throw();
	bool			notify_callback(const http_err_t &http_err,const http_rephd_t &http_rephd
						, socket_full_t *socket_full
						, const bytearray_t &recved_data)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	http_client_t() 		throw();
	~http_client_t()		throw();

	/*************** Setup function	***************************************/
	http_client_t &	set_profile(const http_client_profile_t &profile)	throw();
	http_client_t &	set_itor_arg(const socket_itor_arg_t &itor_arg)		throw();
	http_client_t &	set_client_pool(http_client_pool_t *client_pool)	throw();
	http_err_t	start(const http_reqhd_t &http_reqhd, http_client_cb_t *callback
							, void *userptr)	throw();
							
	/*************** Query function	***************************************/
	const std::set<http_uri_t> &	get_handled_uri_db()	const throw()	{ return handled_uri_db;	}
	http_client_pool_t *		get_client_pool()	const throw()	{ return client_pool;		}
	const http_reqhd_t &		get_http_reqhd()	const throw()	{ return http_reqhd;		}
	const http_uri_t &		get_current_uri()	const throw()	{ return http_reqhd.uri();	}

	/*************** List of friend class	*******************************/
	friend class	http_client_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_CLIENT_HPP__  */




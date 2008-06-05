/*! \file
    \brief Header of the http_listener_cnx_t
    
*/


#ifndef __NEOIP_HTTP_LISTENER_CNX_HPP__ 
#define __NEOIP_HTTP_LISTENER_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_http_listener_cnx_wikidbg.hpp"
#include "neoip_http_err.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_listener_t;
class	pkt_t;

/** \brief class definition for http_listener_cnx_t
 * 
 * - read the data from the responded connection until the http_reqhd_t is known and 
 *   then send it to the proper http_resp_t
 */
class http_listener_cnx_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t, public timeout_cb_t
		, private wikidbg_obj_t<http_listener_cnx_t, http_listener_cnx_wikidbg_init> {
private:
	http_listener_t *	http_listener;	//!< backpointer to the linked http_listener_t
	bytearray_t		recved_data;
	
	/*************** internal function	*******************************/
	bool		autodelete(const std::string &reason = "")	throw();

	/*************** expire_timeout stuff	*******************************/
	timeout_t	expire_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** socket_full_t	***************************************/
	socket_full_t *	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw();
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)	throw();
public:
	/*************** ctor/dtor	***************************************/
	http_listener_cnx_t() 		throw();
	~http_listener_cnx_t()		throw();
	
	/*************** setup function	***************************************/
	http_err_t	start(http_listener_t *http_listener, socket_full_t *socket_full)	throw();

	/*************** list of friend class	*******************************/
	friend class	http_listener_cnx_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_LISTENER_CNX_HPP__  */




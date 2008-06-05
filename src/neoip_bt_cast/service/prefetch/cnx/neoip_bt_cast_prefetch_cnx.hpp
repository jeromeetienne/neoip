/*! \file
    \brief Header of the bt_cast_prefetch_cnx_t
    
*/


#ifndef __NEOIP_BT_CAST_PREFETCH_CNX_HPP__ 
#define __NEOIP_BT_CAST_PREFETCH_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_cast_prefetch_cnx_wikidbg.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_client_cb.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_cast_prefetch_t;
class	bt_cast_prefetch_cnx_t;
class	bt_err_t;

/** \brief class definition for bt_cast_prefetch_cnx_t
 * 
 */
class bt_cast_prefetch_cnx_t : NEOIP_COPY_CTOR_DENY
			, private http_client_cb_t, private socket_full_cb_t
			, private wikidbg_obj_t<bt_cast_prefetch_cnx_t, bt_cast_prefetch_cnx_wikidbg_init> {				
private:
	bt_cast_prefetch_t *	m_cast_prefetch;	//!< backpointer to the linked bt_cast_prefetch_t
	http_uri_t		m_http_uri;
	/*************** Internal function	*******************************/
	bool		autodelete(const std::string &reason = "")	throw();

	/*************** http_client_t	***************************************/
	http_client_t *	m_http_client;
	bool		neoip_http_client_cb(void *cb_userptr, http_client_t &cb_http_client
				, const http_err_t &http_err, const http_rephd_t &http_rephd
				, socket_full_t *socket_full, const bytearray_t &recved_data) throw();

	/*************** socket_full_t	***************************************/
	socket_full_t *	m_socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
					, const socket_event_t &socket_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_prefetch_cnx_t() 	throw();
	~bt_cast_prefetch_cnx_t()	throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_cast_prefetch_t *m_cast_prefetch
						, const http_uri_t &m_http_uri)	throw();

	/*************** setup function	***************************************/
	const http_uri_t &	http_uri()	const throw()	{ return m_http_uri;	}

	/*************** List of friend class	*******************************/
	friend class	bt_cast_prefetch_cnx_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_PREFETCH_CNX_HPP__  */




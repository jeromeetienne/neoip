/*! \file
    \brief Header of the http_sclient_t
    
*/


#ifndef __NEOIP_HTTP_SCLIENT_HPP__ 
#define __NEOIP_HTTP_SCLIENT_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_http_sclient_wikidbg.hpp"
#include "neoip_http_sclient_profile.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_http_sclient_res.hpp"
#include "neoip_http_client_cb.hpp"
#include "neoip_http_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_sclient_mod_vapi_t;
class	http_client_profile_t;
class	http_client_pool_t;
class	http_reqhd_t;
class	socket_itor_arg_t;
class	pkt_t;

/** \brief class definition for http_client
 */
class http_sclient_t : NEOIP_COPY_CTOR_DENY, private http_client_cb_t, private socket_full_cb_t
				, private wikidbg_obj_t<http_sclient_t, http_sclient_wikidbg_init, http_client_cb_t, socket_full_cb_t>
				{
public:	/////////////////////// constant declaration ///////////////////////////
	static const std::string	METAVAR_PREFIX;	//!< the prefix for all metavariables
private:
	http_sclient_profile_t	profile;	//!< the http_sclient_profile_t to use
	http_sclient_res_t	m_current_res;	//!< current http_sclient_res_t (caller one)
	datum_t			data2post;	//!< the data to post (may be null)
	http_sclient_mod_vapi_t*m_mod_vapi;	//!< pointer on the mod_vapi of this http_sclient_t

	/*************** Internal function	*******************************/
	strvar_db_t		m_metavar_db;	//!< the database for the metavar
	void			metavar_db_extract(http_reqhd_t *http_reqhd)	throw();
	

	/*************** http_client_pool_t	*******************************/
	http_client_pool_t *	client_pool;		//!< pointer on the client_pool (externally owned)
	std::set<http_uri_t>	pool_handled_uri_db;
	http_uri_t		pool_current_uri;

	/*************** http_client_t	***************************************/
	http_client_t *	http_client;
	bool		neoip_http_client_cb(void *cb_userptr, http_client_t &cb_http_client
					, const http_err_t &http_err, const http_rephd_t &http_rephd
					, socket_full_t *socket_full, const bytearray_t &recved_data) throw();
	datum_t		neoip_http_client_data2post_cb(void *cb_userptr, http_client_t &cb_http_client
								, size_t offset, size_t maxlen)	throw();

	/*************** socket_full_t	***************************************/
	socket_full_t *	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
					, const socket_event_t &socket_event)	throw();
	bool		handle_recved_data(pkt_t &pkt)	throw();
	
	/*************** Message Length handling	***********************/
	bool		msglen_onclosecnx;	//!< true if the message length is known by cnx close
	bool		msglen_contentlen;	//!< true if the message length is known by Content-Length
	bool		msglen_chunkedenc;	//!< true if the message length is known by chunked encoding
	file_size_t	content_length;		//!< the Content-Length field of the reply header
						//!< (non null IFF msglen_contentlen is true)

	/*************** chunked encoding stuff	*******************************/
	bool		chunkedenc_fully_recved()				const throw();
	void		chunkedenc_decoding()					throw();
	size_t		chunk_headlen(const char *buf_ptr, size_t buf_len)	const throw();
	size_t		chunk_datalen(const char *buf_ptr, size_t buf_len)	const throw();

	/*************** callback stuff	***************************************/
	http_sclient_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback_progress_chunk()			throw();
	bool		notify_callback_short(const http_err_t &cnx_err)	throw();
	bool		notify_callback(const http_sclient_res_t &sclient_res)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	http_sclient_t() 		throw();
	~http_sclient_t()		throw();

	/*************** Setup function	***************************************/
	http_sclient_t &set_profile(const http_sclient_profile_t &profile)	throw();
	http_sclient_t &set_itor_arg(const socket_itor_arg_t &itor_arg)		throw();
	http_sclient_t &set_client_pool(http_client_pool_t *client_pool)	throw();
	http_err_t	start(const http_reqhd_t &http_reqhd, http_sclient_cb_t *callback
					, void *userptr, const datum_t &data2post = datum_t())	throw();
	http_err_t	start(const http_uri_t &http_uri, http_sclient_cb_t *callback
					, void *userptr, const datum_t &data2post = datum_t() )	throw();

	// NOTE: used only by upnp_disc_t to get local_addr.ip_addr()- WIN32 IP_PKTINFO issue
	// - TODO if so it would be better to export only the local_addr from here
	socket_full_t *	get_socket_full()	const throw()	{ return socket_full;	}

	/*************** Query function	***************************************/
	const strvar_db_t &		metavar_db()	const throw()	{ return m_metavar_db;	}
	const http_sclient_res_t &	current_res()	const throw()	{ return m_current_res;	}

	/*************** Action function	*******************************/
	bool		forward_cnx_to_pool(http_client_pool_t *dest_client_pool)	throw();

	/*************** List of friend class	*******************************/
	friend class	http_sclient_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SCLIENT_HPP__  */




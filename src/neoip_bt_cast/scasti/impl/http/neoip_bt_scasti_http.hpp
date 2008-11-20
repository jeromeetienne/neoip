/*! \file
    \brief Header of the bt_scasti_http_t

*/


#ifndef __NEOIP_BT_SCASTI_HTTP_HPP__
#define __NEOIP_BT_SCASTI_HTTP_HPP__
/* system include */
/* local include */
#include "neoip_bt_scasti_http_wikidbg.hpp"
#include "neoip_bt_scasti_http_profile.hpp"
#include "neoip_bt_scasti_cb.hpp"
#include "neoip_bt_scasti_mod_type.hpp"
#include "neoip_bt_scasti_vapi.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_http_client_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_bt_io_write_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_mfile_t;
class	bt_scasti_mod_vapi_t;
class	bt_io_vapi_t;
class	pkt_t;

/** \brief class definition for bt_scasti_http_t
 */
class bt_scasti_http_t : NEOIP_COPY_CTOR_DENY, public bt_scasti_vapi_t
		, private http_client_cb_t, private socket_full_cb_t
		, private bt_io_write_cb_t
		, private wikidbg_obj_t<bt_scasti_http_t, bt_scasti_http_wikidbg_init>  {
private:
	http_uri_t		m_scasti_uri;	//!< the source of the http stream
	bt_io_vapi_t *		m_io_vapi;	//!< the bt_io_vapi_t
	file_size_t		m_cur_offset;	//!< the amount of byte already written
	bt_scasti_http_profile_t profile;	//!< the profile to use for this bt_scasti_http_t

	/*************** bt_scasti_mod_vapi_t	*******************************/
	bt_scasti_mod_vapi_t *	m_mod_vapi;	//!< pointer on the bt_scasti_mod_vapi_t
	bt_err_t		mod_vapi_ctor(const bt_scasti_mod_type_t &mod_type)	throw();
	bool			mod_vapi_notify_callback(const bt_scasti_event_t &event)throw();

	/*************** http_client_t	***************************************/
	http_client_t *		http_client;
	bool			neoip_http_client_cb(void *cb_userptr, http_client_t &cb_http_client
					, const http_err_t &http_err, const http_rephd_t &http_rephd
					, socket_full_t *socket_full, const bytearray_t &recved_data) throw();

	/*************** socket_full_t	***************************************/
	socket_full_t *		socket_full;
	bool			neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
					, const socket_event_t &socket_event)		throw();
	bool			handle_recved_data(pkt_t &pkt)				throw();

	/*************** bt_io_write_t	***************************************/
	bt_io_write_t *		bt_io_write;
	bool 			neoip_bt_io_write_cb(void *cb_userptr, bt_io_write_t &cb_io_write
						, const bt_err_t &bt_err)	throw();

	/*************** callback stuff	***************************************/
	bt_scasti_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback_failed(const bt_err_t &bt_err)		throw();
	bool			notify_callback(const bt_scasti_event_t &scasti_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_scasti_http_t() 		throw();
	~bt_scasti_http_t()		throw();

	/*************** setup function	***************************************/
	bt_scasti_http_t &	set_profile(const bt_scasti_http_profile_t &profile)	throw();
	bt_err_t		start(const http_uri_t &m_scasti_uri, bt_io_vapi_t *m_io_vapi
					, const bt_scasti_mod_type_t &mod_type
					, bt_scasti_cb_t *callback, void *userptr) 	throw();

	/*************** query function	***************************************/
	const file_size_t &	cur_offset()	const throw()	{ return m_cur_offset;	}
	bt_scasti_mod_vapi_t *	mod_vapi()	const throw()	{ return m_mod_vapi;	}

	/*************** List of friend class	*******************************/
	friend class	bt_scasti_http_wikidbg_t;
	friend class	bt_scasti_mod_flv_t;
	friend class	bt_scasti_mod_raw_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SCASTI_HTTP_HPP__  */




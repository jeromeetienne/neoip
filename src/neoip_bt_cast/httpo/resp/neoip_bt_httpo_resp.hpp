/*! \file
    \brief Header of the bt_httpo_resp_t
    
*/


#ifndef __NEOIP_BT_HTTPO_RESP_HPP__ 
#define __NEOIP_BT_HTTPO_RESP_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_httpo_resp_wikidbg.hpp"
#include "neoip_bt_httpo_resp_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_resp_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_httpo_listener_t;
class	bt_httpo_full_t;
class	http_uri_t;
class	http_resp_mode_t;

/** \brief class definition for bt_httpo_resp
 */
class bt_httpo_resp_t : NEOIP_COPY_CTOR_DENY, private http_resp_cb_t
			, private wikidbg_obj_t<bt_httpo_resp_t, bt_httpo_resp_wikidbg_init>
			{
private:
	bt_httpo_listener_t *	m_httpo_listener;	//!< backpointer on bt_httpo_listener_t
	
	/*************** http_resp_t	***************************************/
	http_resp_t *	m_resp_get;		//!< http_resp_t for http_method_t::GET
	http_resp_t *	m_resp_head;		//!< http_resp_t for http_method_t::HEAD
	bool		neoip_http_resp_cb(void *cb_userptr, http_resp_t &cb_http_resp
					, const http_reqhd_t &http_reqhd, socket_full_t *socket_full
					, const bytearray_t &recved_data)	throw();

	/*************** callback stuff	***************************************/
	bt_httpo_resp_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const bt_httpo_event_t &httpo_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_httpo_resp_t() 		throw();
	~bt_httpo_resp_t()		throw();

	/*************** start	***********************************************/
	bt_err_t	start(bt_httpo_listener_t *httpo_listener, const http_uri_t &http_uri
					, const http_resp_mode_t &resp_mode
					, bt_httpo_resp_cb_t *callback, void *userptr)	throw();


	/*************** Query function	***************************************/
	bt_httpo_listener_t *	httpo_listener()	const throw()	{ return m_httpo_listener;}
	
	/*************** List of friend class	*******************************/
	friend class	bt_httpo_full_t;
	friend class	bt_httpo_resp_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPO_RESP_HPP__  */




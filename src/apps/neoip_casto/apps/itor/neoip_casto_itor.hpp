/*! \file
    \brief Header of the \ref casto_itor_t
    
*/


#ifndef __NEOIP_CASTO_ITOR_HPP__ 
#define __NEOIP_CASTO_ITOR_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_casto_itor_wikidbg.hpp"
#include "neoip_casto_itor_cb.hpp"
#include "neoip_bt_cast_mdata_client_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casto_apps_t;
class	bt_httpo_full_t;
class	http_status_t;

/** \brief Handle the itor part for the bt_casto_swarm_t
 */
class casto_itor_t : NEOIP_COPY_CTOR_DENY, private bt_cast_mdata_client_cb_t
			, private wikidbg_obj_t<casto_itor_t, casto_itor_wikidbg_init> {
private:
	casto_apps_t *	m_casto_apps;	//!< backpointer to the attached casto_apps_t
	std::string	m_cast_name;	//!< the cast_name for this itor
	std::string	m_cast_privhash;	//!< the cast_privhash for this itor
	http_uri_t	m_mdata_srv_uri;//!< the http_uri_t for the bt_cast_mdata_server_t

	/*************** bt_cast_mdata_client_t	*******************************/
	bt_cast_mdata_client_t *m_mdata_client;
	bool		neoip_bt_cast_mdata_client_cb(void *cb_userptr, bt_cast_mdata_client_t &cb_mdata_client
					, const bt_err_t &bt_err, const bt_cast_mdata_t &cast_mdata)	throw();

	/*************** bt_httpo_full_t	*******************************/
	std::list<bt_httpo_full_t *>	m_httpo_full_db;	
	void			httpo_full_db_dtor(const http_status_t &status_code
					, const std::string &reason = "")	throw();

	/*************** callback stuff	***************************************/
	casto_itor_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_err_t &bt_err
					, const bt_cast_mdata_t &cast_mdata)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	casto_itor_t()		throw();
	~casto_itor_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(casto_apps_t *casto_apps, const http_uri_t &m_mdata_srv_uri
					, const std::string &m_cast_name
					, const std::string &m_cast_privhash
					, bt_httpo_full_t *httpo_full
					, casto_itor_cb_t *callback, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	const std::string &	cast_name()	const throw()	{ return m_cast_name;	}
	const std::string &	cast_privhash()	const throw()	{ return m_cast_privhash;	}
	const http_uri_t &	mdata_srv_uri()	const throw()	{ return m_mdata_srv_uri;}
	
	/*************** Action function	*******************************/
	void			httpo_full_push(bt_httpo_full_t *httpo_full)	throw();
	bt_httpo_full_t *	httpo_full_pop()				throw();

	/*************** List of friend class	*******************************/
	friend class	casto_itor_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTO_ITOR_HPP__ */











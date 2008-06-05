/*! \file
    \brief Header of the \ref oload_itor_t
    
*/


#ifndef __NEOIP_OLOAD_ITOR_HPP__ 
#define __NEOIP_OLOAD_ITOR_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_oload_itor_wikidbg.hpp"
#include "neoip_oload_itor_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_bt_lnk2mfile_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	oload_apps_t;
class	bt_httpo_full_t;
class	bt_lnk2mfile_profile_t;
class	http_status_t;

/** \brief Handle the itor part for the bt_oload_swarm_t
 */
class oload_itor_t : NEOIP_COPY_CTOR_DENY, private bt_lnk2mfile_cb_t
			, private wikidbg_obj_t<oload_itor_t, oload_itor_wikidbg_init> {
private:
	oload_apps_t *	oload_apps;	//!< backpointer to the attached oload_apps_t
	http_uri_t	m_inner_uri;	//!< the inner_uri for this oload_itor_t

	/*************** Internal function	*******************************/
	static bt_err_t	lnk2mfile_profile_update(bt_lnk2mfile_profile_t &lnk2mfile_profile
						, const http_uri_t &nested_uri)	throw();
	
	/*************** bt_lnk2mfile_t	***************************************/
	bt_lnk2mfile_t *bt_lnk2mfile;
	bool 		neoip_bt_lnk2mfile_cb(void *cb_userptr, bt_lnk2mfile_t &cb_bt_lnk2mfile
					, const bt_err_t &bt_err, const bt_lnk2mfile_type_t &lnk2mfile_type
					, const bt_mfile_t &bt_mfile)		throw();

	/*************** bt_httpo_full_t	*******************************/
	std::list<bt_httpo_full_t *>	m_httpo_full_db;	
	void		httpo_full_db_dtor(const http_status_t &http_status
				, const std::string &reason = ""
				, const http_uri_t &redir_uri = http_uri_t())	throw();

	/*************** callback stuff	***************************************/
	oload_itor_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_err_t &bt_err
						, const bt_mfile_t &bt_mfile)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	oload_itor_t()		throw();
	~oload_itor_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(oload_apps_t *oload_apps, const http_uri_t &nested_uri
					, bt_httpo_full_t *httpo_full
					, oload_itor_cb_t *callback, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	const http_uri_t &	inner_uri()	const throw()	{ return m_inner_uri;	}
	const http_uri_t &	nested_uri()	const throw();
	
	/*************** Action function	*******************************/
	void			httpo_full_push(bt_httpo_full_t *httpo_full)	throw();
	bt_httpo_full_t *	httpo_full_pop()				throw();

	/*************** List of friend class	*******************************/
	friend class	oload_itor_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_ITOR_HPP__ */











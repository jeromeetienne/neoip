/*! \file
    \brief Header of the \ref btrelay_itor_t
    
*/


#ifndef __NEOIP_BTRELAY_ITOR_HPP__ 
#define __NEOIP_BTRELAY_ITOR_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_btrelay_itor_wikidbg.hpp"
#include "neoip_btrelay_itor_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_bt_lnk2mfile_cb.hpp"
#include "neoip_bt_cast_mdata_client_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	btrelay_apps_t;
class	bt_httpo_full_t;

/** \brief Handle the itor part for the bt_btrelay_stat_t
 */
class btrelay_itor_t : NEOIP_COPY_CTOR_DENY, private bt_lnk2mfile_cb_t
			, private bt_cast_mdata_client_cb_t
			, private wikidbg_obj_t<btrelay_itor_t, btrelay_itor_wikidbg_init> {
private:
	btrelay_apps_t*	btrelay_apps;	//!< backpointer to the attached btrelay_apps_t

	/*************** bt_lnk2mfile_t	*******************************/
	bt_lnk2mfile_t *bt_lnk2mfile;
	bool 		neoip_bt_lnk2mfile_cb(void *cb_userptr, bt_lnk2mfile_t &cb_bt_lnk2mfile
					, const bt_err_t &bt_err, const bt_lnk2mfile_type_t &lnk2mfile_type
					, const bt_mfile_t &bt_mfile)		throw();

	/*************** bt_cast_mdata_client_t	*******************************/
	bt_cast_mdata_client_t *mdata_client;
	bool			neoip_bt_cast_mdata_client_cb(void *cb_userptr, bt_cast_mdata_client_t &cb_mdata_client
					, const bt_err_t &bt_err, const bt_cast_mdata_t &cast_mdata)	throw();
					
	/*************** callback stuff	***************************************/
	btrelay_itor_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback_failure(const bt_err_t &bt_err)		throw();
	bool			notify_callback_success(const bt_mfile_t &bt_mfile
						, const bt_cast_mdata_t &cast_mdata)	throw();
	bool			notify_callback(const bt_err_t &bt_err, const bt_mfile_t &bt_mfile
						, const bt_cast_mdata_t &cast_mdata)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	btrelay_itor_t()		throw();
	~btrelay_itor_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(btrelay_apps_t *btrelay_apps, const std::string &link_type
					, const std::string &link_addr
					, btrelay_itor_cb_t *callback, void *userptr)	throw();

	/*************** List of friend class	*******************************/
	friend class	btrelay_itor_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTRELAY_ITOR_HPP__ */











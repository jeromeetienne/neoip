/*! \file
    \brief Header of the \ref bt_lnk2mfile_t class

*/


#ifndef __NEOIP_BT_LNK2MFILE_HPP__ 
#define __NEOIP_BT_LNK2MFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_lnk2mfile_cb.hpp"
#include "neoip_bt_lnk2mfile_profile.hpp"
#include "neoip_bt_lnk2mfile_type.hpp"
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_datum.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_err_t;

/** \brief The object to run the neoip_get apps
 */
class bt_lnk2mfile_t : NEOIP_COPY_CTOR_DENY, private http_sclient_cb_t
					, private zerotimer_cb_t {
private:
	bt_lnk2mfile_type_t	m_link_type;	//!< the link_type
	std::string		link_addr;	//!< the link address
	bt_lnk2mfile_profile_t	m_profile;	//!< the profile to use for this object
	bt_mfile_t		bt_mfile;	//!< contain the bt_mfile_t to build
	file_size_t		static_filelen;	//!< contains the content size IIF link_type="static"

	/*************** zerotimer_t	***************************************/
	zerotimer_t		poststart_zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw();

	/*************** meta_data stuff	*******************************/
	datum_t			metadata_datum;		//!< contain the meta_data read from file or http
	bool			notify_metadata()		throw();
	bt_err_t		process_metadata_torrent()	throw();
	bt_err_t		process_metadata_metalink()	throw();
	bt_err_t		process_metadata_static()	throw();
	
	/*************** Internal function	*******************************/
	void			guess_type_from_addr()	throw();

	/*************** http_client_t	***************************************/
	http_sclient_t *	http_sclient;
	bool 			neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
					, const http_sclient_res_t &sclient_res)	throw();
						
	/*************** callback stuff	***************************************/
	bt_lnk2mfile_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
	bool		notify_callback_succeed()				throw();
	bool		notify_callback_failed(const bt_err_t &bt_err)		throw();
	bool		notify_callback_failed(const std::string &reason)	throw();
	bool		notify_callback(const bt_err_t &bt_err, const bt_lnk2mfile_type_t &lnk2mfile_type
					, const bt_mfile_t &bt_mfile)		throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_lnk2mfile_t()		throw();
	~bt_lnk2mfile_t()		throw();
	
	/*************** setup function	***************************************/
	bt_lnk2mfile_t &profile(const bt_lnk2mfile_profile_t &m_profile)	throw();
	bt_err_t	start(const bt_lnk2mfile_type_t &link_type
					, const std::string &link_addr
					, bt_lnk2mfile_cb_t *callback, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	const bt_lnk2mfile_type_t &	link_type()	const throw()	{ return m_link_type;	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_LNK2MFILE_HPP__  */


 

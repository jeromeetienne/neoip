/*! \file
    \brief Header of the bt_alloc_t
    
*/


#ifndef __NEOIP_BT_ALLOC_HPP__ 
#define __NEOIP_BT_ALLOC_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_alloc_profile.hpp"
#include "neoip_bt_alloc_policy.hpp"
#include "neoip_bt_alloc_cb.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_file_awrite_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	file_aio_t;

/** \brief class definition for bt_alloc
 */
class bt_alloc_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t, private file_awrite_cb_t {
private:
	bt_alloc_profile_t	profile;	//!< the profile to use for this bt_alloc_t
	bt_alloc_policy_t 	alloc_policy;	//!< the mode of this bt_alloc_t
	bt_mfile_t		bt_mfile;	//!< the bt_mfile_t which is allocated
	size_t			cur_file_idx;	//!< the index of the current local file to process in the 
						//!< bt_mfile_t::subfile_arr().
	file_size_t		cur_file_off;	//!< the next file offset within the current file

	
	/*************** zerotimer_t	***************************************/
	zerotimer_t		zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw();

	/*************** Internal function	*******************************/
	bool			launch_next_file()					throw();
	bool			launch_next_write()					throw();
	bt_err_t		create_dir_if_needed(const file_path_t &dir_name)	throw();

	/*************** file_awrite_t	***************************************/
	file_aio_t *		file_aio;
	file_awrite_t *		file_awrite;
	bool			neoip_file_awrite_cb(void *cb_userptr, file_awrite_t &cb_file_awrite
							, const file_err_t &file_err)	throw();

	/*************** created_paths stuff	*******************************/
	std::list<file_path_t>	created_paths;	//!< queue of all the created path (file/directory)
	void			created_paths_add(const file_path_t &file_path)	throw();
	void			created_paths_cleanup()				throw();

	/*************** callback stuff	***************************************/
	bt_alloc_cb_t *		callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback_succeed()				throw();
	bool			notify_callback_failed(const std::string &reason)	throw();
	bool			notify_callback_failed(const bt_err_t &bt_err)		throw();
	bool			notify_callback(const bt_err_t &bt_err)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	bt_alloc_t() 		throw();
	~bt_alloc_t()		throw();

	/*************** Setup function	***************************************/
	bt_alloc_t &	set_profile(const bt_alloc_profile_t &profile)	throw();		
	bt_err_t	start(const bt_mfile_t &bt_mfile, const bt_alloc_policy_t &alloc_policy
					, bt_alloc_cb_t *callback, void *userptr)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_ALLOC_HPP__  */




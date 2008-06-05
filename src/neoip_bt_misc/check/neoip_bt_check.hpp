/*! \file
    \brief Header of the bt_check_t
    
*/


#ifndef __NEOIP_BT_CHECK_HPP__ 
#define __NEOIP_BT_CHECK_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_check_policy.hpp"
#include "neoip_bt_check_cb.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_bt_piece_cpuhash_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_io_vapi_t;

/** \brief class definition for bt_check
 */
class bt_check_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t, private bt_piece_cpuhash_cb_t {
private:
	bt_check_policy_t	check_policy;	//!< the bt_check_policy_t for this bt_check_t
	bt_mfile_t		bt_mfile;	//!< the bt_mfile_t which is checked
	bt_io_vapi_t *		bt_io_vapi;	//!< the bt_io_vapi_t to read the data
	bt_swarm_resumedata_t	caller_resumedata;//!< the bt_swarm_resumedata_t from the caller
	
	bitfield_t		tocheck_bfield;	//!< the bitfield_t of piece_idx to check
	bitfield_t		isavail_bfield;	//!< the bitfield_t of piece_idx to avail
	size_t			cur_pieceidx;

	/*************** zerotimer_t	***************************************/
	zerotimer_t		zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw();

	/*************** Internal function	*******************************/
	bt_err_t		internal_start(const bt_check_policy_t &check_policy
					, const bt_mfile_t &bt_mfile
					, const bt_swarm_resumedata_t &swarm_resumedata
					, bt_io_vapi_t *bt_io_vapi
					, bt_check_cb_t *callback, void *userptr)		throw();	
	bt_err_t		check_all_file_access()						throw();
	static bt_err_t		check_one_file_access(const bt_mfile_subfile_t &mfile_subfile)	throw();
	void			tocheck_bfield_ctor_partial()					throw();
	void			tocheck_bfield_ctor_every()					throw();
	bool			launch_piece_cpuhash()						throw();

	/*************** bt_piece_cpuhash_t	*******************************/
	bt_piece_cpuhash_t *	piece_cpuhash;
	bool 			neoip_bt_piece_cpuhash_cb(void *cb_userptr, bt_piece_cpuhash_t &cb_piece_cpuhash
					, const bt_err_t &bt_err, const bt_id_t &piecehash) throw();

	/*************** callback stuff	***************************************/
	bt_check_cb_t *		callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback_err(const bt_err_t &bt_err)	throw()
						{ return notify_callback(bt_err, bt_swarm_resumedata_t());}
	bool			notify_callback(const bt_err_t &bt_err
						, const bt_swarm_resumedata_t &swarm_resumedata)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	bt_check_t() 		throw();
	~bt_check_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(const bt_check_policy_t &check_policy, const bt_swarm_resumedata_t &swarm_resumedata
					, bt_io_vapi_t *bt_io_vapi
					, bt_check_cb_t *callback, void *userptr)	throw();
	bt_err_t	start(const bt_check_policy_t &check_policy, const bt_mfile_t &bt_mfile
					, bt_io_vapi_t *bt_io_vapi
					, bt_check_cb_t *callback, void *userptr)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CHECK_HPP__  */




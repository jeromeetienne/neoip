/*! \file
    \brief Header of the bt_mfile_cpuhash_t
    
*/


#ifndef __NEOIP_BT_MFILE_CPUHASH_HPP__ 
#define __NEOIP_BT_MFILE_CPUHASH_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_mfile_cpuhash_cb.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_piece_cpuhash_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_io_vapi_t;

/** \brief class definition for bt_mfile_cpuhash
 */
class bt_mfile_cpuhash_t : NEOIP_COPY_CTOR_DENY, private bt_piece_cpuhash_cb_t {
private:
	bt_mfile_t		bt_mfile;	//!< the bt_mfile_t on which the action occurs
	bt_io_vapi_t *		bt_io_vapi;	//!< pointer on the bt_io_vapi_t
	bt_id_arr_t		piecehash_arr;	//!< the bt_id_arr_t to store the generated piecehash

	/*************** Internal function	*******************************/
	void			launch_next_piece()	throw();

	/*************** bt_piece_cpuhash_t	*******************************/
	bt_piece_cpuhash_t *	piece_cpuhash;
	bool 			neoip_bt_piece_cpuhash_cb(void *cb_userptr, bt_piece_cpuhash_t &cb_piece_cpuhash
					, const bt_err_t &bt_err, const bt_id_t &piecehash) throw();

	/*************** callback stuff	***************************************/
	bt_mfile_cpuhash_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_err_t &bt_err
						, const bt_id_arr_t &piecehash_arr) throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	bt_mfile_cpuhash_t() 		throw();
	~bt_mfile_cpuhash_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(const bt_mfile_t &bt_mfile, bt_io_vapi_t *bt_io_vapi
					, bt_mfile_cpuhash_cb_t *callback, void *userptr)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_MFILE_CPUHASH_HPP__  */




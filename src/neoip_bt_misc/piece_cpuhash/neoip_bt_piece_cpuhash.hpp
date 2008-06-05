/*! \file
    \brief Header of the bt_piece_cpuhash_t
    
*/


#ifndef __NEOIP_BT_PIECE_CPUHASH_HPP__ 
#define __NEOIP_BT_PIECE_CPUHASH_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_piece_cpuhash_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_io_read_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_mfile_t;
class	bt_io_vapi_t;

/** \brief class definition for bt_piece_cpuhash
 */
class bt_piece_cpuhash_t : NEOIP_COPY_CTOR_DENY, private bt_io_read_cb_t {
private:
	/*************** bt_io_read_t	***************************************/
	bt_io_read_t *		bt_io_read;
	bool 			neoip_bt_io_read_cb(void *cb_userptr, bt_io_read_t &cb_io_read
					, const bt_err_t &bt_err, const datum_t &read_data) throw();

	/*************** callback stuff	***************************************/
	bt_piece_cpuhash_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_err_t &bt_err, const bt_id_t &piecehash) throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	bt_piece_cpuhash_t(size_t piece_idx, const bt_mfile_t &bt_mfile, bt_io_vapi_t *bt_io_vapi
					, bt_piece_cpuhash_cb_t *callback, void *userptr) 	throw();
	~bt_piece_cpuhash_t()		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PIECE_CPUHASH_HPP__  */




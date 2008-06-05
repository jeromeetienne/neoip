/*! \file
    \brief Header of the bt_io_pfile_read_t
    
*/


#ifndef __NEOIP_BT_IO_PFILE_READ_HPP__ 
#define __NEOIP_BT_IO_PFILE_READ_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_io_read_cb.hpp"
#include "neoip_bt_io_read.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_prange_arr.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_file_aread_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_io_pfile_t;
class	file_aio_t;
class	file_range_t;

/** \brief class definition for bt_io_pfile_read
 */
class bt_io_pfile_read_t : NEOIP_COPY_CTOR_DENY, public bt_io_read_t, private file_aread_cb_t
					, private zerotimer_cb_t {
private:
	bt_io_pfile_t *	io_pfile;	//!< backpointer on the attached bt_io_pfile_t
	bt_prange_arr_t	prange_arr;	//!< the bt_prange_arr_t on which this read operats
	size_t		prange_idx;	//!< the index of the current bt_prange_t in the bt_prange_arr_t
	bytearray_t	data_queue;	//!< the data already read
	
	/*************** zerotimer_t	***************************************/
	bt_err_t	zerotimer_param;
	zerotimer_t	zerotimer_err;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** Internal function	*******************************/
	bt_err_t	launch_next_prange()	throw();
	
	/*************** file_aread_t	***************************************/
	file_aio_t *	file_aio;
	file_aread_t *	file_aread;
	bool		neoip_file_aread_cb(void *cb_userptr, file_aread_t &cb_file_aread
					, const file_err_t &file_err, const datum_t &data)	throw();

	/*************** callback stuff	***************************************/
	bt_io_read_cb_t*callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const bt_err_t &bt_err, const datum_t &io_pfile_read_data) throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_io_pfile_read_t(bt_io_pfile_t *io_pfile, const file_range_t &totfile_range
				, bt_io_read_cb_t *callback, void *userptr) 		throw();
	~bt_io_pfile_read_t()								throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_PFILE_READ_HPP__  */




/*! \file
    \brief Header of the bt_io_sfile_write_t
    
*/


#ifndef __NEOIP_BT_IO_SFILE_WRITE_HPP__ 
#define __NEOIP_BT_IO_SFILE_WRITE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_io_write_cb.hpp"
#include "neoip_bt_io_write.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_iov_arr.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_file_awrite_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_io_sfile_t;
class	file_aio_t;
class	file_range_t;

/** \brief class definition for bt_io_sfile_write
 */
class bt_io_sfile_write_t : NEOIP_COPY_CTOR_DENY, public bt_io_write_t, private file_awrite_cb_t
					, private zerotimer_cb_t {
private:
	bt_io_sfile_t *	io_sfile;	//!< backpointer on the attached bt_io_sfile_t
	bt_iov_arr_t	iov_arr;	//!< the bt_iov_arr_t on which this write operats
	size_t		iov_idx;	//!< the index of the current bt_iov_t in the bt_iov_arr_t
	datum_t		data2write;	//!< the data to write
	file_size_t	m_written_len;	//!< the length already written
	
	/*************** zerotimer_t	***************************************/
	bt_err_t	zerotimer_param;
	zerotimer_t	zerotimer_err;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();
	
	/*************** Internal function	*******************************/
	bt_err_t	launch_next_iov()	throw();
	
	/*************** file_awrite_t	***************************************/
	file_aio_t *	file_aio;
	file_awrite_t *	file_awrite;
	bool		neoip_file_awrite_cb(void *cb_userptr, file_awrite_t &cb_file_awrite
						, const file_err_t &file_err)	throw();

	/*************** callback stuff	***************************************/
	bt_io_write_cb_t*callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const bt_err_t &bt_err) 		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_io_sfile_write_t(bt_io_sfile_t *io_sfile, const file_range_t &totfile_range
				, const datum_t &data2write
				, bt_io_write_cb_t *callback, void *userptr) 		throw();
	~bt_io_sfile_write_t()								throw();

	/*************** query function	***************************************/
	const file_size_t &	written_len()	const throw()	{ return m_written_len;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_SFILE_WRITE_HPP__  */




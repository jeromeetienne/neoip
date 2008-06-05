/*! \file
    \brief Class to handle the bt_io_sfile_read_t

*/

/* system include */
/* local include */
#include "neoip_bt_io_sfile_read.hpp"
#include "neoip_bt_io_sfile.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_file_aio.hpp"
#include "neoip_file_aread.hpp"
#include "neoip_file_range.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_io_sfile_read_t::bt_io_sfile_read_t(bt_io_sfile_t *io_sfile, const file_range_t &totfile_range
				, bt_io_read_cb_t *callback, void *userptr)	throw()
{
	const bt_mfile_t &	bt_mfile	= io_sfile->bt_mfile();
	// copy the parameter
	this->io_sfile	= io_sfile;
	this->callback	= callback;
	this->userptr	= userptr;
	// link to the bt_io_sfile_t
	io_sfile->read_dolink(this);

	// update the statistics bt_io_stats_t in bt_io_pfile_t
	io_sfile->stats().update_for_read(totfile_range);
	
	// set some field
	iov_idx		= 0;
	file_aio	= NULL;
	file_aread	= NULL;

	// compute the matching bt_iov_arr_t from bt_mfile_t
	iov_arr		= bt_mfile.get_iov_arr(totfile_range);	
	
	// launch the first iov
	bt_err_t	bt_err;
	bt_err		= launch_next_iov();
	if( bt_err.failed() ){
		// if there is an error in the ctor, notify it via a zerotimer not to notify reccursively
		zerotimer_param	= bt_err;
		zerotimer_err.append(this, NULL);
		return;
	}
}

/** \brief Destructor
 */
bt_io_sfile_read_t::~bt_io_sfile_read_t()		throw()
{
	// unlink from the bt_io_sfile_t
	io_sfile->read_unlink(this);
	// delete the file_aread_t if needed
	nipmem_zdelete	file_aread;
	// delete the file_aio_t if needed
	nipmem_zdelete	file_aio;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       err_zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 * 
 * - NOTE: used only to report the error from the ctor
 */
bool	bt_io_sfile_read_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// sanity check - the zerotimer_param MUST NOT be null
	DBG_ASSERT( !zerotimer_param.is_null() );
	// notify the error
	return notify_callback(zerotimer_param, datum_t());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        launch_next_iov
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the bt_iov_t at index iov_idx in the bt_iov_arr_t 
 */
bt_err_t	bt_io_sfile_read_t::launch_next_iov()	throw()
{
	const bt_mfile_t&bt_mfile	= io_sfile->bt_mfile();
	const bt_iov_t &bt_iov		= iov_arr[iov_idx];
	file_path_t	file_path	= bt_mfile.subfile_arr()[bt_iov.subfile_idx()].local_path();
	file_err_t	file_err;
	// sanity check - the file_aio and file_aread MUST be NULL
	DBG_ASSERT( !file_aio );
	DBG_ASSERT( !file_aread );
	
	// start a file_aio_t
	file_aio	= nipmem_new file_aio_t();
	file_err	= file_aio->start(file_path, file_mode_t::READ);
	if( file_err.failed() )	return bt_err_from_file(file_err);

	// start the file_aread_t
	file_aread	= nipmem_new file_aread_t(file_aio);
	file_err	= file_aread->start(bt_iov.subfile_beg(), bt_iov.length(), this, NULL);
	if( file_err.failed() )	return bt_err_from_file(file_err);

	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        file_aread_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	bt_io_sfile_read_t::neoip_file_aread_cb(void *cb_userptr, file_aread_t &cb_file_aread
				, const file_err_t &file_err, const datum_t &aread_data)	throw()
{
	// log to debug
	KLOG_DBG("enter file_err=" << file_err << " aread_data.size()=" << aread_data.size());

	// if the file_aread_t failed, notify the error to the caller
	if( file_err.failed() )	return notify_callback(bt_err_from_file(file_err), datum_t());	

	// add the just read data to the bytearray_t
	// TODO why no preallocation here
	data_queue.append(aread_data);
	
	// delete the file_aread_t
	nipmem_zdelete	file_aread;
	// delete the file_aio_t
	nipmem_zdelete	file_aio;
	
	// update the iov_idx
	iov_idx++;

	// if all the bt_iov_t has been successfully performed, notify the caller
	// TODO why no datum_t::NOCOPY here ? useless memcpy - bt_io_pfile_t has it
	if( iov_idx == iov_arr.size() )	return notify_callback(bt_err_t::OK, data_queue.to_datum());	
	
	// else launch the next bt_iov_t
	bt_err_t	bt_err;
	bt_err		= launch_next_iov();
	if( bt_err.failed() )		return notify_callback( bt_err, datum_t() );

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_io_sfile_read_t::notify_callback(const bt_err_t &bt_err, const datum_t &readv_data)throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_io_read_cb(userptr, *this, bt_err, readv_data);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - tokeep MUST be false as caller MUST always delete this object in the callback
	DBG_ASSERT( !tokeep );
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END






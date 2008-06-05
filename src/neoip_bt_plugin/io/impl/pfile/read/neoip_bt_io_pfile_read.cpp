/*! \file
    \brief Class to handle the bt_io_pfile_read_t

\par TODO improve memory performance
- a lot of room for improvement
- the data are ALLOCATED + COPIED before being notified !!!
  - aka no datum_t::NOCOPY in the notification
- the data_queue IS NOT preallocated
- LATER: i think it is fixed now... to check

*/

/* system include */
/* local include */
#include "neoip_bt_io_pfile_read.hpp"
#include "neoip_bt_io_pfile.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_file_aio.hpp"
#include "neoip_file_aread.hpp"
#include "neoip_bt_prange_arr.hpp"
#include "neoip_bt_unit.hpp"
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
bt_io_pfile_read_t::bt_io_pfile_read_t(bt_io_pfile_t *io_pfile, const file_range_t &totfile_range
				, bt_io_read_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->io_pfile	= io_pfile;
	this->callback	= callback;
	this->userptr	= userptr;
	// link to the bt_io_pfile_t
	io_pfile->read_dolink(this);

	// update the statistics bt_io_stats_t in bt_io_pfile_t
	io_pfile->stats().update_for_read(totfile_range);

	// set some field
	prange_idx	= 0;	
	file_aio	= NULL;
	file_aread	= NULL;

	// compute the bt_prange_arr_t matching this totfile_range
	prange_arr	= io_pfile->totfile_to_prange_arr(totfile_range);

#if 1	// TODO sketching for preallocation
	// preallocate the data_queue to avoid reallocation during the read
	data_queue	= bytearray_t(totfile_range.length().to_size_t());
#endif

	// launch the first prange
	bt_err_t	bt_err;
	bt_err		= launch_next_prange();
	if( bt_err.failed() ){
		// if there is an error in the ctor, notify it via a zerotimer not to notify reccursively
		zerotimer_param	= bt_err;
		zerotimer_err.append(this, NULL);
		return;
	}
}

/** \brief Destructor
 */
bt_io_pfile_read_t::~bt_io_pfile_read_t()		throw()
{
	// unlink from the bt_io_pfile_t
	io_pfile->read_unlink(this);
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
bool	bt_io_pfile_read_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// sanity check - the zerotimer_param MUST NOT be null
	DBG_ASSERT( !zerotimer_param.is_null() );
	// notify the error
	return notify_callback(zerotimer_param, datum_t());
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        launch_next_prange
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the bt_prange_t at index prange_idx in the bt_prange_arr_t 
 */
bt_err_t	bt_io_pfile_read_t::launch_next_prange()	throw()
{
	const bt_prange_t &	bt_prange	= prange_arr[prange_idx];
	file_path_t		file_path	= io_pfile->piece_filepath(bt_prange.piece_idx());
	file_err_t		file_err;
	// sanity check - the file_aio and file_aread MUST be NULL
	DBG_ASSERT( !file_aio );
	DBG_ASSERT( !file_aread );
	
	// start a file_aio_t
	file_aio	= nipmem_new file_aio_t();
	file_err	= file_aio->start(file_path, file_mode_t::READ);
	if( file_err.failed() )	return bt_err_from_file(file_err);

	// start the file_aread_t
	file_aread	= nipmem_new file_aread_t(file_aio);
	file_err	= file_aread->start(bt_prange.offset(), bt_prange.length(), this, NULL);
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
bool	bt_io_pfile_read_t::neoip_file_aread_cb(void *cb_userptr, file_aread_t &cb_file_aread
				, const file_err_t &file_err, const datum_t &aread_data)	throw()
{
	// log to debug
	KLOG_DBG("enter file_err=" << file_err << " aread_data.size()=" << aread_data.size());

	// if the file_aread_t failed, notify the error to the caller
	if( file_err.failed() )	return notify_callback(bt_err_from_file(file_err), datum_t());	

#if 1	// TODO sketching for preallocation
	// sanity check - the recved_data is supposed to be preallocated
	DBG_ASSERT( data_queue.tail_freelen() >= aread_data.length());
#endif
	// add the just read data to the bytearray_t
	data_queue.append(aread_data);
	
	// delete the file_aread_t
	nipmem_zdelete	file_aread;
	// delete the file_aio_t
	nipmem_zdelete	file_aio;
	
	// update the prange_idx
	prange_idx++;

	// if all the bt_prange_t has been successfully performed, notify the caller
	if( prange_idx == prange_arr.size() )
		return notify_callback(bt_err_t::OK, data_queue.to_datum(datum_t::NOCOPY));	
	
	// else launch the next bt_prange_t
	bt_err_t	bt_err;
	bt_err		= launch_next_prange();
	if( bt_err.failed() )		return notify_callback( bt_err, datum_t() );

	// notify the data to the caller
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_io_pfile_read_t::notify_callback(const bt_err_t &bt_err, const datum_t &readv_data)throw()
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






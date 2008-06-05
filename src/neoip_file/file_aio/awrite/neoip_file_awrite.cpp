/*! \file
    \brief Class to handle the file_awrite_t

*/

/* system include */
#include <unistd.h>
/* local include */
#include "neoip_file_awrite.hpp"
#include "neoip_file_aio.hpp"
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
file_awrite_t::file_awrite_t(file_aio_t *file_aio)		throw()
{
	// sanity check - the file_aio MUST NOT be NULL
	DBG_ASSERT( file_aio );
	// copy the parameter
	this->file_aio	= file_aio;
	// sanity check - the file_mode_t of file_aio_t MUST include file_mode_t::WRITE
	DBG_ASSERT( file_aio->get_mode().include( file_mode_t::WRITE ) );
	// link it to the attached object
	file_aio->awrite_dolink(this);
}

/** \brief Destructor
 */
file_awrite_t::~file_awrite_t()		throw()
{
	// log to debug
	KLOG_DBG("END WRITE of " << file_aio->get_path().to_os_path_string() 
				<< " from " << file_offset << " to " << data2write.length());
	// unlink it from the attached object
	file_aio->awrite_unlink(this);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
file_err_t	file_awrite_t::start(const file_size_t &file_offset, const datum_t &data2write
					, file_awrite_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// log to debug
	KLOG_DBG("START WRITE of " << file_aio->get_path().to_os_path_string() 
				<< " from " << file_offset << " to " << data2write.length());
	// copy the parameter
	this->file_offset	= file_offset;
	this->data2write	= bytearray_t(data2write);
	this->callback		= callback;
	this->userptr		= userptr;
	// zero the writtelen cursor
	writtenlen		= 0;
	
	// launch zerotimer for the first operation
	zerotimer.append(this, NULL);
	
	// return no error
	return file_err_t::OK;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	file_awrite_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	const file_aio_profile_t &	profile = file_aio->get_profile();
	// log to debug
	KLOG_DBG("enter");

	// compute the amount of data to read in this iteration
	file_size_t	iterlen= file_size_t(data2write.size()) - writtenlen;
	if( iterlen > profile.awrite_iterlen() )	iterlen = profile.awrite_iterlen();

	// seek to the proper position
	off_t	offset	= file_size_t(file_offset + writtenlen).to_off_t();
	off_t	seek_res= lseek( file_aio->get_fd(), offset, SEEK_SET);
	if( seek_res != offset ){
		file_err_t file_err = file_err_t(file_err_t::ERROR, "cant lseek due to " + neoip_strerror(errno) );
		return notify_callback( file_err );
	}

	// write the data to the file
	char *	buf	= data2write.char_ptr() + writtenlen.to_size_t();
	ssize_t	writeres= write(file_aio->get_fd(), buf, iterlen.to_size_t());
	if( writeres != iterlen.to_ssize_t() ){
		file_err_t file_err = file_err_t(file_err_t::ERROR, "cant write due to " + neoip_strerror(errno) );
		return notify_callback( file_err );
	}
	
	// update the writtenlen
	writtenlen	+= writeres;

	// if the file_awrite_t is completed, notify the caller
	if( writtenlen == data2write.size() )	return notify_callback(file_err_t::OK);

	// relaunch zerotimer for the next operation
	zerotimer.append(this, NULL);
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool file_awrite_t::notify_callback(const file_err_t &file_err)			throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_file_awrite_cb(userptr, *this, file_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END






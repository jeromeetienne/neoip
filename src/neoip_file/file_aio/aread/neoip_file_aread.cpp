/*! \file
    \brief Class to handle the file_aread_t

- TODO to use fdwatch_t on a non blocking fd ?
  - will have less CPU overhead because less zerotimer_t
  - wont be blocking as it is now (e.g. problem with file over nfs)
  - but fd on file ok for fdwatch_t ?
  
*/

/* system include */
#include <unistd.h>
/* local include */
#include "neoip_file_aread.hpp"
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
file_aread_t::file_aread_t(file_aio_t *file_aio)		throw()
{
	// copy the parameter
	this->file_aio	= file_aio;
	// sanity check - the file_mode_t of file_aio_t MUST include read
	DBG_ASSERT( file_aio->get_mode().include( file_mode_t::READ ) );
	// link it to the attached object
	file_aio->aread_dolink(this);
}

/** \brief Destructor
 */
file_aread_t::~file_aread_t()		throw()
{
	// log to debug
	KLOG_DBG("END READ of " << file_aio->get_path().to_os_path_string() << " from "
					 << file_offset << " to " << aread_len);
	// unlink it from the attached object
	file_aio->aread_unlink(this);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
file_err_t	file_aread_t::start(const file_size_t &offset, const file_size_t& len
				, file_aread_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("START READ of " << file_aio->get_path().to_os_path_string() << " from "
					 << file_offset << " to " << aread_len);
	// copy the parameter
	this->file_offset	= offset;
	this->aread_len		= len;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// preallocate the recved_data to avoid reallocation during the read
	// TODO currently the preallocation is bugged - see serial_t::operator=
	recved_data		= bytearray_t(len.to_size_t());

	// launch zerotimer for the first operation
	zerotimer.append(this, NULL);
	// return no error
	return file_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			zerotimer callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	file_aread_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	const file_aio_profile_t &	profile = file_aio->get_profile();
	// log to debug
	KLOG_DBG("enter");

	// compute the amount of data to read in this iteration
	file_size_t	iterlen= aread_len - recved_data.size();
	if( iterlen > profile.aread_iterlen() )	iterlen = profile.aread_iterlen();

	// seek to the proper position
	off_t	offset	= file_offset.get_value() + recved_data.size();
	off_t	seek_res= lseek( file_aio->get_fd(), offset, SEEK_SET);
	if( seek_res != offset ){
		file_err_t file_err = file_err_t(file_err_t::ERROR, "cant lseek due to " + neoip_strerror(errno) );
		return notify_callback( file_err, datum_t() );
	}

/* TODO
 * - read directly in the buffer
 *   - instead of the alloca + memcpy
 * - try to pass the fd in non block before read
 *   - do the read the max possible
 *   - pass the fd in blocking again
 */
#if 1

#if 1	// TODO currently the preallocation is bugged - see serial_t::operator=
	// sanity check - the recved_data is supposed to be preallocated
	DBG_ASSERT( recved_data.tail_freelen() >= iterlen.to_size_t());
#endif
	// allocate the room in recved_data for the data to read
	char *	buf	= recved_data.tail_alloc(iterlen.to_size_t());
	// attempt to read them
	ssize_t	read_res= read(file_aio->get_fd(), buf, iterlen.to_size_t());
	if( read_res != iterlen.to_ssize_t() ){
		file_err_t file_err = file_err_t(file_err_t::ERROR, "cant read due to " + neoip_strerror(errno) );
		return notify_callback( file_err, datum_t() );
	}
	// NOTE: no need to compensate the recved_data.tail_alloc() as the read() always read the whole
#else
	// read the data from the file
	void *	buf	= nipmem_alloca(iterlen.to_size_t());
	ssize_t	read_res= read(file_aio->get_fd(), buf, iterlen.to_size_t());
	// log to debug
	KLOG_DBG("read_res=" << read_res << " iterlen=" << iterlen << " file_aio=" << *file_aio
				<< " recved_data=" << recved_data << " file_offset=" << file_offset);
	if( read_res != iterlen.to_ssize_t() ){
		file_err_t file_err = file_err_t(file_err_t::ERROR, "cant read due to " + neoip_strerror(errno) );
		return notify_callback( file_err, datum_t() );
	}
	// add the just read data to the bytearray_t
	recved_data.append(buf, read_res);
#endif
	// if the file_aread_t is completed, notify the caller
	if( recved_data.size() == aread_len.to_size_t() )
		return notify_callback(file_err_t::OK, recved_data.to_datum(datum_t::NOCOPY));

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
bool file_aread_t::notify_callback(const file_err_t &file_err, const datum_t &data)throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_file_aread_cb(userptr, *this, file_err, data);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END






/*! \file
    \brief Class to handle the file_sio_t

- TODO the read/write doesnt handle partial operation
  - i.e. if a signal occurs during the read/write it will fails instead of retry

*/

/* system include */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
/* local include */
#include "neoip_file_sio.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_oswarp.hpp"
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
file_sio_t::file_sio_t()		throw()
{
	// zero some field
	file_fd	= -1;
}

/** \brief Destructor
 */
file_sio_t::~file_sio_t()		throw()
{
	// close the file if needed
	if( file_fd >= 0 )	close(file_fd);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
file_err_t	file_sio_t::start(const file_path_t &file_path, const file_mode_t &file_mode
					, const file_perm_t &file_perm)	throw()
{
	// sanity check - the file_mode_t MUST NOT be null
	DBG_ASSERT( !file_mode.is_null() );
	// copy the parameter
	this->file_path	= file_path;
	this->file_mode	= file_mode;

	// do the open(2) of the file
	file_fd		= file_oswarp_t::open(file_path, file_mode, file_perm);
	if( file_fd < 0 )	return file_err_t(file_err_t::ERROR, neoip_strerror(errno));

	// return no error
	return file_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief seek the file_sio_t to the proper position
 */
file_err_t	file_sio_t::seek(const file_size_t &file_offset)	const throw()
{
	// seek to the proper position
	off_t	offset	= file_offset.get_value();
	off_t	seek_res= lseek(get_fd(), offset, SEEK_SET);
	if( seek_res != offset )
		return file_err_t(file_err_t::ERROR, "cant lseek due to " + neoip_strerror(errno) );
	// return no error
	return file_err_t::OK;
}

/** \brief Write the data to the file_sio_t at the given offset
 */
file_err_t	file_sio_t::write(const datum_t &data)	const throw()
{
	// write the data to the file
	ssize_t	writeres= ::write(get_fd(), data.get_ptr(), data.size());
	if( writeres != (ssize_t)data.size() )
		return file_err_t(file_err_t::ERROR, "cant write due to " + neoip_strerror(errno) );
	// return no error
	return file_err_t::OK;
}

/** \brief Read EXACTLY len byte from the file_sio_t
 */
file_err_t	file_sio_t::read(const file_size_t &len, datum_t &data_out)const throw()
{
	// sanity check - len MUST be ssize_t_ok
	DBG_ASSERT( len.is_ssize_t_ok() );
	// init the data out
	data_out	= datum_t(len.to_size_t());
	// read the data from the file
	ssize_t	read_res= ::read(get_fd(), data_out.get_ptr(), data_out.size());
	if( read_res != (ssize_t)len.to_ssize_t() )
		return file_err_t(file_err_t::ERROR, "cant read due to " + neoip_strerror(errno) );
	// return no error	
	return file_err_t::OK;
}

/** \brief Read at most maxlen byte from the file_sio_t
 */
file_err_t	file_sio_t::read_max(const file_size_t &maxlen, datum_t &data_out)	const throw()
{
	void *	buffer	= (void *)nipmem_alloca(maxlen.to_size_t());
	// init the data out
	data_out	= datum_t();
	// read the data from the file
	ssize_t	read_res= ::read(get_fd(), buffer, maxlen.to_size_t());
	if( read_res < 0 )
		return file_err_t(file_err_t::ERROR, "cant read due to " + neoip_strerror(errno) );
	// set the data_out
	data_out	= datum_t(buffer, read_res);
	// return no error	
	return file_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//               some static shortcut to read/write a whole file
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a whole file and return its data in data_out
 */
file_err_t	file_sio_t::readall(const file_path_t &file_path, datum_t &data_out
					, const file_size_t &_maxlen)		throw()
{
	file_size_t	maxlen	= _maxlen.is_null() ? 512*1024 : _maxlen;
	file_err_t	file_err;
	file_sio_t	file_sio;
	file_size_t	file_size;
	// start the file_sio_t
	file_err	= file_sio.start(file_path, file_mode_t::READ);
	if( file_err.failed() )		return file_err;

	// read all the data
	file_err	= file_sio.read_max(maxlen, data_out);
	if( file_err.failed() )		return file_err;

	// return no error
	return file_err_t::OK;
}

/** \brief Write a datum_t in a file
 * 
 * - if the file doesnt exist it is created
 * - if it exists it is truncated
 * - in short after this operation the content of the whole file is the datum_t
 */
file_err_t	file_sio_t::writeall(const file_path_t &file_path, const datum_t &data
				, const file_perm_t &file_perm, const file_mode_t &file_mode)	throw()
{
	file_err_t	file_err;
	file_sio_t	file_sio;
	// start the file_sio_t
	file_err	= file_sio.start(file_path, file_mode, file_perm);
	if( file_err.failed() )		return file_err;
	// read all the data
	file_err	= file_sio.write(data);
	if( file_err.failed() )		return file_err;
	// return no error
	return file_err_t::OK;
}



NEOIP_NAMESPACE_END






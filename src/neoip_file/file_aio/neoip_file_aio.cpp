/*! \file
    \brief Class to handle the file_aio_t

- TODO to use fdwatch_t on a non blocking fd ?
  - will have less CPU overhead because less zerotimer_t
  - wont be blocking as it is now (e.g. problem with file over nfs)
  - but fd on file ok for fdwatch_t ?

*/

/* system include */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* local include */
#include "neoip_file_aio.hpp"
#include "neoip_file_aread.hpp"
#include "neoip_file_awrite.hpp"
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
file_aio_t::file_aio_t()		throw()
{
	// zero some field
	file_fd	= -1;
}

/** \brief Destructor
 */
file_aio_t::~file_aio_t()		throw()
{
	// close all pending file_aread_t
	DBGNET_ASSERT( aread_db.empty() );
	while(!aread_db.empty())	nipmem_delete aread_db.front();	
	// close all pending file_awrite_t
	DBGNET_ASSERT( awrite_db.empty() );
	while(!awrite_db.empty())	nipmem_delete awrite_db.front();	
	// close the file if needed
	if( file_fd >= 0 )		close(file_fd);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
file_aio_t &	file_aio_t::set_profile(const file_aio_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check().succeed() );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
file_err_t	file_aio_t::start(const file_path_t &file_path, const file_mode_t &file_mode
					, const file_perm_t &file_perm)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the file_mode_t MUST NOT be null
	DBG_ASSERT( !file_mode.is_null() );
	// copy the parameter
	this->file_path	= file_path;
	this->file_mode	= file_mode;

	// do the open(2) of the file
	file_fd	= file_oswarp_t::open(file_path, file_mode, file_perm);
	if( file_fd == -1 )	return file_err_t(file_err_t::ERROR, "Cant open " + file_path.to_os_path_string() + " due to " + neoip_strerror(errno));

	// return no error
	return file_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object into a std::string
 */
std::string	file_aio_t::to_string()	const throw()
{
	std::ostringstream      oss;
	// build the string
	oss << "file_path=" << get_path();
	oss << " ";
	oss << "file_mode=" << get_mode();
	// return the built string
	return oss.str();	
	
}

NEOIP_NAMESPACE_END






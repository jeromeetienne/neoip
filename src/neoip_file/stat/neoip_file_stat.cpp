/*! \file
    \brief Implementation of \ref file_stat_t to read directories
 
*/

/* system include */

/* local include */
#include "neoip_file_stat.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       open
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor for the class
 * 
 * - it opens the file_path_t if it is not null
 */
file_stat_t::file_stat_t(const file_path_t &file_path)		throw()
{	
	// set is_initialized to false
	is_initialized	= false;
	// open the file_path if not null
	if( !file_path.is_null() )	this->open(file_path);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       open
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Do a stat on this file_path_t
 */
file_err_t	file_stat_t::open(const file_path_t &file_path)			throw()
{	
	// sanity check - the file_path MUST NOT be NULL
	DBG_ASSERT( !file_path.is_null() );
	// close the current if needed
	if( !is_null() )	this->close();
	// get the stat structure
	int	err	= stat(file_path.to_os_path_string().c_str(), &stat_stt);
	// if an error occurs, report it now
	if( err )	return file_err_from_errno("Cant stat() " + file_path.to_os_path_string());

	// set is_initialized to true
	is_initialized	= true;
	
	// return no error
	return file_err_t::OK;
}

/** \brief Constructor for the class
 */
void	file_stat_t::close()			throw()
{	
	// zero the stat_stt
	memset( &stat_stt, 0, sizeof(stat_stt) );
	// zero is_initialized
	is_initialized	= false;
}


NEOIP_NAMESPACE_END




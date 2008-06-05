/*! \file
    \brief Class to handle the bt_io_sfile_t
    
\par Brief Description
\ref bt_io_sfile_t is a bt_io_vapi_t implementation which store the files on the local
filesystem. It is the typical behaviour for a bt client, aka download from the network and
store on the local filesystem.

\par PRO/CON
- PRO: store the data directly in the bt_mfile_subfile_t::local_path, so they can
  be used directly by the users
- CON: impossible to remove part of a file
  - would require to be able to punch hole in sparse file
    - apparently it is possible on window but not in linux
    - a linux proposal sys_punch existed for years but is not included in the 
      main kernel. apparently because it is hard to make it compatible with mmap

\par Possible Improvement
- Use a disk cache which keep a given among of data in RAM before storing them on 
  disk, thus it would be faster even if it uses more memory
- Possible solution for the remove() function
  - maintain a file_size_inval_t which contains all the intervals which have been
    asked for removal but it was impossible as it was only part of a file.
  - each time a remove() add new range in this interval, test if there is 
    a whole file being removed
  - each time a write happen, this interval MUST be updated
  - NOTE: ok something along that line could works, the issue is similar in 
    bt_io_pfile_t and bt_io_sfile_t

*/

/* system include */
/* local include */
#include "neoip_bt_io_sfile.hpp"
#include "neoip_bt_io_sfile_read.hpp"
#include "neoip_bt_io_sfile_write.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_prange.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_io_sfile_t::bt_io_sfile_t()		throw()
{
	// initialize some variables
	m_io_mode	= bt_io_mode_t::SFILE;
}

/** \brief Destructor
 */
bt_io_sfile_t::~bt_io_sfile_t()		throw()
{
	// close all pending bt_io_sfile_read_t
	DBGNET_ASSERT( read_db.empty() );
	while( !read_db.empty() )	nipmem_delete read_db.front();	
	// close all pending bt_io_sfile_write_t
	DBGNET_ASSERT( write_db.empty() );
	while( !write_db.empty() )	nipmem_delete write_db.front();	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_io_sfile_t::start(const bt_mfile_t &m_bt_mfile)	throw()
{
	// sanity check - the bt_mfile_t MUST be is_sfile_ok();
	DBG_ASSERT( m_bt_mfile.is_sfile_ok() );
	// copy the parameter
	this->m_bt_mfile	= m_bt_mfile;
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       bt_io_vapi_t function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Construct a bt_io_read_t request
 */
bt_io_read_t *	bt_io_sfile_t::read_ctor(const file_range_t &totfile_range
					, bt_io_read_cb_t *callback, void *userptr)	throw()
{
	return nipmem_new bt_io_sfile_read_t(this, totfile_range, callback, userptr);
}

/** \brief Construct a bt_io_write_t request
 */
bt_io_write_t *	bt_io_sfile_t::write_ctor(const file_range_t &totfile_range, const datum_t &data2write
					, bt_io_write_cb_t *callback, void *userptr)	throw()
{
	return nipmem_new bt_io_sfile_write_t(this, totfile_range, data2write, callback, userptr);
}

/** \brief remove a totfile_range in sync
 */
bt_err_t	bt_io_sfile_t::remove(const file_range_t &totfile_range)		throw()
{
	// this is impossible to remove piece with bt_io_sfile_t
	// - would require to work on a filesystem which handle sparse file
	// - would require to be able to punch hole in file
	//   - apparently it is possible on window but not in linux
	//   - a linux proposal sys_punch existed for years but is not included in the 
	//     main kernel
	DBG_ASSERT( 0 );
	// return bt_err_t::ERROR
	return bt_err_t::ERROR;
}

NEOIP_NAMESPACE_END






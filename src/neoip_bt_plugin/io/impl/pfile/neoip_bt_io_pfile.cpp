/*! \file
    \brief Class to handle the bt_io_pfile_t

\par TODO
- TODO handle the profile().has_circularidx in the whole class and subclass
  - see bt_io_pfile_write_t in the start() function
  - apparently all rely on a single function 
    - prange_arr	= bt_unit_t::totfile_range_to_prange_arr(totfile_range, bt_mfile);
    - this is true in bt_io_pfile_write_t and bt_io_pfile_read_t
  - if bt_mfile contains the property has_circularidx
    - all the function who handle pieceidx and has the bt_mfile_t may be able
      to handle the has_circularidx transparently
  - ISSUE: file_range_t clearly expect a beg() <= end()
    - so the responsability of who does the warparound is important 
    - what about : file_range coming in bt_io_vapi_t may exceed the bt_mfile.totfile_len
      - if bt_mfile.has_circularidx is false, report an error
      - else handle the circularidx ?

\par Brief Description
\ref bt_io_pfile_t is a \ref bt_io_vapi_t implementation store the data in a special directory
with a file per piece. It is possible to read any previously written data, even accross 
several pieces.

\par PRO/CON
- PRO: being able to remove from the disk data already downloaded
  - see the sys_punch issue on a normal sparse file
- CON: special format so it can be used directly by the user
  - for this use the bt_io_sfile_t
- Conclusion: it works when when the data are stored for delivering data thru
  a stream like http connection

\par Limitations
- NOTE: the remove() function MUST be piece aligned.
  - if it is not, the function will assert
  - currently it is not an issue as all delete are done by pieceidx anyway

\par Possible Improvement
- Use a disk cache which keep a given among of data in RAM before storing them on 
  disk, thus it would be faster even if it uses more memory
- possible solution about the limitations of remove MUST be piece aligned
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
#include "neoip_bt_io_pfile.hpp"
#include "neoip_bt_io_pfile_read.hpp"
#include "neoip_bt_io_pfile_write.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_prange_arr.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_file_utils.hpp"
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
bt_io_pfile_t::bt_io_pfile_t()		throw()
{
	// initialize some variables
	m_io_mode	= bt_io_mode_t::PFILE;
}

/** \brief Destructor
 */
bt_io_pfile_t::~bt_io_pfile_t()		throw()
{
	// close all pending bt_io_pfile_read_t
	DBGNET_ASSERT( read_db.empty() );
	while( !read_db.empty() )	nipmem_delete read_db.front();	
	// close all pending bt_io_pfile_write_t
	DBGNET_ASSERT( write_db.empty() );
	while( !write_db.empty() )	nipmem_delete write_db.front();	
	// remove the temporary files created by this bt_io_pfile_t
	remove_temp_files();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_io_pfile_t &	bt_io_pfile_t::profile(const bt_io_pfile_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be check().succeed()
	DBG_ASSERT( p_profile.check().succeed() );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_io_pfile_t::start(const bt_mfile_t &m_bt_mfile)	throw()
{
	// sanity check - the profile MUST be check().succeed()
	DBG_ASSERT( profile().check().succeed() );	
	// sanity check - the bt_mfile_t MUST be is_pfile_ok();
	DBG_ASSERT( m_bt_mfile.is_pfile_ok() );
	// copy the parameter
	this->m_bt_mfile	= m_bt_mfile;

	// init the m_file_exist
	// - NOTE: this assumes no piece exists before now - this may change in the future
	m_file_exist	= bitfield_t(m_bt_mfile.nb_piece());
	
	// build the dest_dirname file_path_t
	dest_dirname	= profile().dest_dirpath() / m_bt_mfile.infohash().to_canonical_string();
	// create the destination directory
	file_err_t	file_err;
	file_err	= file_utils_t::create_directory(dest_dirname, file_utils_t::NO_RECURSION);
	if( file_err.failed() ){
		// mark the directory creation as not successfull
		dest_dirname	= file_path_t();
		// return the error
		return bt_err_from_file(file_err);
	}
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Remove the temporary files created by this bt_io_pfile_t
 * 
 * - WARNING: this perform a blocking disk operation on all pieces, so can be quite long
 *   - aka file_utils_t::remove_file() which is blocking and the number of piece may be large
 *   - aka dangerous in a event loop context
 */
void	bt_io_pfile_t::remove_temp_files()	const throw()
{
	// if bt_mfile is null, do nothing
	if( bt_mfile().is_null() )	return;
	// if dest_dirname is null, the ctor has not been run or failed, so dont remove files
	if( dest_dirname.is_null() )	return;
	// try to remove all the files name for all the pieces, even if they dont exists...
	for(size_t piece_idx = 0; piece_idx < bt_mfile().nb_piece(); piece_idx++){
		file_path_t	file_path;
		// if the file for this piece_idx doesnt exist, goto the next
		if( m_file_exist[piece_idx] == false )	continue;
		// get the destination file_path_t for this piece_idx
		file_path	= piece_filepath(piece_idx);
		// remove the files
		file_utils_t::remove_file(file_path);
		// NOTE: the error is purposely ignored - in case the file wasnt created
	}
	// remove the directory itself
	file_utils_t::remove_directory(dest_dirname, file_utils_t::NO_RECURSION);			
}

/** \brief Return the file_path_t for this piece_idx
 */
file_path_t	bt_io_pfile_t::piece_filepath(size_t piece_idx)	const throw()
{
	std::ostringstream	dest_basename;
	// build the dest_basename
	dest_basename << "piece" << std::setfill('0') << std::setw(8) << piece_idx << ".tmp";
	// build and return the dest_path
	return dest_dirname / dest_basename.str();
}

/** \brief Return the bt_prange_arr_t for this totfile_range
 * 
 * - it handle the profile.has_circularidx()
 */
bt_prange_arr_t	bt_io_pfile_t::totfile_to_prange_arr(const file_range_t &totfile_range_orig)
										const throw()
{
	const file_size_t &	totfile_size	= bt_mfile().totfile_size();
	file_range_t		totfile_range	= totfile_range_orig;
	
	// if the totfile_range.beg is > totfile_size, shift it 
	if( totfile_range.beg() >= totfile_size ){
		// sanity check - profile().has_circularidx() MUST be set
		DBG_ASSERT( profile().has_circularidx() );
		// compute the delta to shift		
		file_size_t	delta	= totfile_range.beg() - (totfile_range.beg() % totfile_size);
		// update the totfile_range to be less than totfile_size 
		totfile_range	= file_range_t(totfile_range.beg()-delta, totfile_range.end() - delta);
	}
	// sanity check - at this point, totfile_range.beg() MUST be < totfile_size
	DBG_ASSERT( totfile_range.beg() < totfile_size );

	// if the totfile_range.end is < than totfile_size, compute it now
	// - this means there is no need for has_circularidx special case
	if( totfile_range.end() < totfile_size )
		return bt_unit_t::totfile_range_to_prange_arr(totfile_range, bt_mfile());

	// sanity check - profile().has_circularidx() MUST be set
	DBG_ASSERT( profile().has_circularidx() );

	// split the totfile_range in 2 sequential elements
	file_range_t	totfile_range1(totfile_range.beg() % totfile_size, totfile_size-1);
	file_range_t	totfile_range2(0, totfile_range.end() % totfile_size);

	// sanity check - totfile_range1 MUST be fully included in totfile_range
	DBG_ASSERT( totfile_range.fully_include(totfile_range1) );
	// the splited totfile_range1/2 MUST be < totfile_size
	DBG_ASSERT( totfile_range1.end() < totfile_size );
	DBG_ASSERT( totfile_range2.end() < totfile_size );

	// compute the prange_arr for the splitted totfile_range 
	// - NOTE: the order of the prange_arr_t MUST be preserved 
	//   - aka the pieces at the end of totfile_range MUST be at the end of prange_arr
	//   - this is required as bt_io_pfile_read/write_t are processing
	//     bt_prange_arr_t sequentially
	bt_prange_arr_t	prange_arr;
	prange_arr	= bt_unit_t::totfile_range_to_prange_arr(totfile_range1, bt_mfile());
	prange_arr	+=bt_unit_t::totfile_range_to_prange_arr(totfile_range2, bt_mfile());
	// return the just built bt_prange_arr_t
	return prange_arr;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       bt_io_vapi_t function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Construct a bt_io_read_t request
 */
bt_io_read_t *	bt_io_pfile_t::read_ctor(const file_range_t &totfile_range
					, bt_io_read_cb_t *callback, void *userptr)	throw()
{
	return nipmem_new bt_io_pfile_read_t(this, totfile_range, callback, userptr);
}

/** \brief Construct a bt_io_write_t request
 */
bt_io_write_t *	bt_io_pfile_t::write_ctor(const file_range_t &totfile_range, const datum_t &data2write
					, bt_io_write_cb_t *callback, void *userptr)	throw()
{
	return nipmem_new bt_io_pfile_write_t(this, totfile_range, data2write, callback, userptr);
}

/** \brief remove a totfile_range in sync
 */
bt_err_t	bt_io_pfile_t::remove(const file_range_t &totfile_range)		throw()
{
	bt_prange_arr_t	prange_arr;
	file_path_t	file_path;
	file_err_t	file_err;
	// compute the bt_prange_arr_t matching this totfile_range
	prange_arr	= bt_unit_t::totfile_range_to_prange_arr(totfile_range, bt_mfile());
	// go thru the whole prange_arr
	for(size_t prange_idx = 0; prange_idx < prange_arr.size(); prange_idx++){
		bt_prange_t &	bt_prange	= prange_arr[prange_idx];
		// sanity check - the bt_prange MUST be the full piece
		// - currently it is not possible to remove part of a file
		if( bt_prange.length() != bt_unit_t::pieceidx_to_piecelen(bt_prange.piece_idx(), bt_mfile())){
			KLOG_ERR("bt_prange=" << bt_prange);
			KLOG_ERR("bt_mfile.nb_piece()=" << bt_mfile().nb_piece());
			KLOG_ERR("piecelen=" << bt_unit_t::pieceidx_to_piecelen(bt_prange.piece_idx(), bt_mfile()));
		}
		DBG_ASSERT( bt_prange.length() == bt_unit_t::pieceidx_to_piecelen(bt_prange.piece_idx(), bt_mfile()));

		// if the file for this piece_idx doesnt exist, goto the next
		if( m_file_exist[bt_prange.piece_idx()] == false )	continue;

		// get the destination file_path_t for this piece_idx
		file_path	= piece_filepath(bt_prange.piece_idx());
		// remove the files
		file_err	= file_utils_t::remove_file(file_path);
		if( file_err.failed() )	return bt_err_from_file(file_err);
	}
	// return no error
	return bt_err_t::OK;	
}

NEOIP_NAMESPACE_END






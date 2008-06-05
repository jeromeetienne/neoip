/*! \file
    \brief Definition of the \ref bt_unit_t class

\par TODO renaming for better readability
- an example
  - size_t	pieceidx_beg	= bt_unit_t::totfile_to_pieceidx(chunk_beg, bt_mfile);
  - size_t	pieceidx_beg	= bt_mfile.pieceidx_for_totfile(chunk_beg);
- this implies renaming the function and to include inline them in the bt_mfile_t


*/

/* system include */
/* local include */
#include "neoip_bt_unit.hpp"
#include "neoip_bt_prange_arr.hpp"
#include "neoip_bt_pieceavail.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      non inlined functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a totfile_range in a bt_prange_arr_t
 */
bt_prange_arr_t	bt_unit_t::totfile_range_to_prange_arr(const file_range_t &totfile_range_orig
						, const bt_mfile_t &bt_mfile)	throw()
{
	file_range_t	totfile_range	= totfile_range_orig;
	bt_prange_arr_t	prange_arr;
	
	// sanity check - the totfile_range.end MUST be < bt_mfile.totfile_size()
	DBG_ASSERT( totfile_range.end() < bt_mfile.totfile_size() );
	
	// loop for ever - the exit condition is inside the loop
	while( true ){
		// get the piece_idx for the begining of the totfile_range
		size_t		piece_idx	= bt_unit_t::totfile_to_pieceidx(totfile_range.beg(), bt_mfile);
		// get the piece_range for this piece_idx
		file_range_t	piece_range	= bt_unit_t::pieceidx_to_totfile_range(piece_idx, bt_mfile);
		// clamp the piece_range with the totfile_range
		piece_range.clamped_by(totfile_range);
		// add this piece_range to the prange_arr
		prange_arr	+= bt_prange_t::from_totfile_range(piece_range, bt_mfile);
		// if the end of piece_range is equal to the end of totfile_range, leave the loop
		if( piece_range.end() == totfile_range.end() )	break;
		// update the totfile_range.beg() to start 1 byte after the piece_range.end()
		totfile_range.beg( piece_range.end() + 1 );		
	}
	// return the just built bt_prange_arr_t
	return prange_arr;
}

/** \brief return the available length of data for this bt_pieceavail_t
 * 
 * - the bt_mfile_t is needed to know the length of the last piece
 */
file_size_t	bt_unit_t::totfile_avail(const bt_pieceavail_t &pieceavail
						, const bt_mfile_t &bt_mfile)	throw()
{
	file_size_t	avl_size 	= 0;
	size_t		nb_piece	= pieceavail.nb_piece(); 
	// sanity check - the bt_mfile_t MUST be fully_init
	DBG_ASSERT( bt_mfile.is_fully_init() );
	// sanity check - the bt_pieceavail_t MUST matching the bt_mfile
	DBG_ASSERT( pieceavail.nb_piece() == bt_mfile.nb_piece() );
	// sanity check - the size() MUST NOT be zero
	DBG_ASSERT( pieceavail.nb_piece() != 0 );
	// compute the avl_size for the pieces fully used - aka all but the last
	// - TODO this seems like a special cast which should be handled here
	//   what about using bt_unit_t::pieceidx_to_piecelen()
	for(size_t i = 0; i < nb_piece - 1; i++ ){
		// if this piece is not available, skip it
		if( !pieceavail.is_avail(i) )	continue;
		// count this piece
		avl_size	+= bt_mfile.piecelen();	
	}
	// compute the avl_size for the last pieces
	if( pieceavail.is_avail(nb_piece-1) ){
		file_size_t	last_piece_off	= file_size_t(nb_piece-1) * bt_mfile.piecelen();
		file_size_t	last_piece_size	= bt_mfile.totfile_size() - last_piece_off;
		avl_size	+= last_piece_size;
	}
	// return the computed available size
	return avl_size;
}


NEOIP_NAMESPACE_END



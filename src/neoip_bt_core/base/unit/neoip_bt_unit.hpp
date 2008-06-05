/*! \file
    \brief Header of the bt_unit_t class

- TODO put alias of all this in the bt_mfile_t object
  - similarly to how bt_id_t use the bt_peerid_helper_t
    
*/


#ifndef __NEOIP_BT_UNIT_HPP__ 
#define __NEOIP_BT_UNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_mfile.hpp"
#include "neoip_file_range.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_prange_arr_t;
class	bt_pieceavail_t;

/** \brief static helpers to manipulate unit in neoip_bt
 */
class bt_unit_t {
public:
	/** \brief Convert a totfile offset to a piece_idx for a given bt_mfile_t
	 */
	static size_t	totfile_to_pieceidx(const file_size_t &totfile_offset, const bt_mfile_t &bt_mfile)	throw()
	{
		return file_size_t(totfile_offset / bt_mfile.piecelen()).to_size_t();		
	}

	/** \brief Convert a totfile offset to a pieceoff for a given bt_mfile_t
	 */
	static size_t	totfile_to_pieceoff(const file_size_t &totfile_offset, const bt_mfile_t &bt_mfile)	throw()
	{
		return file_size_t(totfile_offset % bt_mfile.piecelen()).to_size_t();		
	}
	
	/** \brief Convert a pieceidx to a piecelen
	 * 
	 * - it hande the special case of the last piece which may be shorter than other ones
	 */
	static size_t	pieceidx_to_piecelen(size_t piece_idx, const bt_mfile_t &bt_mfile)	throw()
	{
		// sanity check - piece_idx MUST be < nb_piece()
		DBG_ASSERT( piece_idx < bt_mfile.nb_piece() );
		// sanity check - bt_mfile.nb_piece() MUST be > 0
		DBG_ASSERT( bt_mfile.nb_piece() > 0 );		
		// if piece_idx is the last piece AND the last piece is partial
		// Then its size is the totfile_size % piece_len
		if( piece_idx == bt_mfile.nb_piece()-1 && (bt_mfile.totfile_size() % bt_mfile.piecelen()) != 0 )
			return file_size_t(bt_mfile.totfile_size() % bt_mfile.piecelen()).to_size_t();
		// else it is piece_len()
		return bt_mfile.piecelen();
	}

	/** \brief Convert a pieceidx to a piecebeg - aka totfile_offset of the first byte of this piece
	 */
	static file_size_t	pieceidx_to_piecebeg(size_t piece_idx, const bt_mfile_t &bt_mfile)	throw()
	{
		// sanity check - piece_idx MUST be < nb_piece()
		DBG_ASSERT( piece_idx < bt_mfile.nb_piece() );
		// sanity check - nb_piece MUST be > 0
		// - NOTE: it is impossible to return a valid value, as this return a offset of a existing byte
		//   of this piece, and no piece exist
		DBG_ASSERT( bt_mfile.nb_piece() > 0 );
		// else return piece_len() * piece_idx
		return file_size_t(bt_mfile.piecelen()) * piece_idx;		
	}

	/** \brief Convert a pieceidx to a pieceend - aka totfile_offset of the last byte of this piece
	 */
	static file_size_t	pieceidx_to_pieceend(size_t piece_idx, const bt_mfile_t &bt_mfile)	throw()
	{
		// sanity check - piece_idx MUST be < nb_piece()
		DBG_ASSERT( piece_idx < bt_mfile.nb_piece() );
		// sanity check - nb_piece MUST be > 0
		// - NOTE: it is impossible to return a valid value, as this return a offset of a existing byte
		//   of this piece, and no piece exist
		DBG_ASSERT( bt_mfile.nb_piece() > 0 );
		// if piece_idx is the last piece, the offset of its last byte is totfile_size - 1
		if( piece_idx == bt_mfile.nb_piece()-1 )	return	bt_mfile.totfile_size() - 1;
		// else return (piece_len() * (piece_idx + 1)) -1
		return (file_size_t(bt_mfile.piecelen()) * (piece_idx+1)) - 1;	
	}
	
	/** \brief Convert a piece_idx to a totfile_range
	 */
	static file_range_t	pieceidx_to_totfile_range(size_t piece_idx, const bt_mfile_t &bt_mfile)	throw()
	{
		file_range_t	file_range;
		// build the file_range_t
		file_range.beg	( pieceidx_to_piecebeg(piece_idx, bt_mfile) );
		file_range.end	( pieceidx_to_pieceend(piece_idx, bt_mfile) );
		// return the just build file_range_t
		return file_range;
	}
	
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	//               non inlined function
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	static bt_prange_arr_t	totfile_range_to_prange_arr(const file_range_t &totfile_range
						, const bt_mfile_t &bt_mfile)	throw();		
	static file_size_t	totfile_avail(const bt_pieceavail_t &pieceavail
						, const bt_mfile_t &bt_mfile)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UNIT_HPP__  */




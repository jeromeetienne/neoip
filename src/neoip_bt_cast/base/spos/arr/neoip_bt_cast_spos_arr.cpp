/*! \file
    \brief Definition of the \ref bt_cast_spos_arr_t
    
*/

/* system include */
/* local include */
#include "neoip_bt_cast_spos_arr.hpp"
#include "neoip_bt_cast_pidx.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the bt_cast_spos_t contains a given byte_offset
 */
bool	bt_cast_spos_arr_t::contain(const file_size_t &byte_offset)	throw()
{
	// go thru the whole array
	for(size_t i = 0; i < size(); i++){
		const bt_cast_spos_t &	cast_spos	= (*this)[i];
		// if this item byte_offset matches, return true
		if( cast_spos.byte_offset() == byte_offset)	return true;
	}
	// if this point is reached, return false
	return false;
}

/** \brief Return a bt_cast_spos_arr_t with all the byte_offset within the pieceq
 */
bt_cast_spos_arr_t	bt_cast_spos_arr_t::within_pieceq(size_t pieceq_beg, size_t pieceq_end
						, const bt_mfile_t &bt_mfile)	const throw()
{
	bt_cast_pidx_t		cast_pidx	= bt_cast_pidx_t().modulo(bt_mfile.nb_piece());
	bt_cast_spos_arr_t	result;
	// go thru the whole cast_spos_arr
	for(size_t i = 0; i < size(); i++){
		const bt_cast_spos_t &	cast_spos	= (*this)[i];
		const file_size_t &	byte_offset	= cast_spos.byte_offset();
		size_t			pieceidx;
		// determine the pieceidx of this bt_cast_spos_t
		pieceidx	= bt_unit_t::totfile_to_pieceidx(byte_offset, bt_mfile);
		// if this byte_offset pieceidx is NOT in pieceq, goto the next
		if( !cast_pidx.index(pieceidx).is_in(pieceq_beg, pieceq_end) )	continue;
		// add this bt_cast_spos_t to the result
		result		+= cast_spos;
	}
	// return the just built result
	return result;
}

/** \brief Return the bt_cast_spos_t with the closest casti_date 
 */
const bt_cast_spos_t &	bt_cast_spos_arr_t::closest_casti_date(const date_t &casti_date)const throw()
{
	size_t	best_idx	= 0;
	int64_t	best_delta	= std::numeric_limits<int64_t>::max();
	// sanity check - the object MUST NOT be empty 
	DBG_ASSERT( !empty() );
	// go thru the whole array
	for(size_t idx = 0; idx < size(); idx++){
		const bt_cast_spos_t &	cast_spos	= (*this)[idx];
		int64_t			curr_delta;
		// compute the delta between the current cast_spos and casti_date
		curr_delta	= cast_spos.casti_date().to_uint64() - casti_date.to_uint64();
		curr_delta	= std::abs(curr_delta);
		// if the current delta is lower than the best_delta, update the best
		if( curr_delta < best_delta ){
			best_idx	= idx;
			best_delta	= curr_delta;
		}
	}
	// return the best bt_cast_spos_t
	return (*this)[best_idx];
}

NEOIP_NAMESPACE_END;







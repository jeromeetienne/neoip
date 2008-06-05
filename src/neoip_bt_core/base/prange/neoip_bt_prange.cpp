/*! \file
    \brief Definition of the \ref bt_prange_t

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_bt_prange.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_file_range.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the totfile_beg of this bt_prange_t
 */
file_size_t	bt_prange_t::totfile_beg(const bt_mfile_t &bt_mfile)		const throw()
{
	// sanity check - the length() MUST be 0
	DBG_ASSERT( length() > 0 );
	// compute the totfile_beg
	return file_size_t(piece_idx()) * bt_mfile.piecelen() + offset();
}

/** \brief Return the totfile_end of this bt_prange_t
 */
file_size_t	bt_prange_t::totfile_end(const bt_mfile_t &bt_mfile)		const throw()
{
	// sanity check - the length() MUST be 0
	DBG_ASSERT( length() > 0 );
	// return the totfile_beg + length() -1 
	return totfile_beg(bt_mfile) + length() - 1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a bt_prange_t to a totfile file_range_t
 */
file_range_t	bt_prange_t::to_totfile_range(const bt_mfile_t &bt_mfile)	const throw()
{
	file_range_t	file_range;
	// set the resulting file_range_t
	file_range.beg	( file_size_t(piece_idx()) * bt_mfile.piecelen() + offset() );
	file_range.end	( file_range.beg() + length() - 1 );
	// return the resulting file_range_t
	return file_range;		
}

/** \brief Convert a totfile file_range_t to a bt_prange_t
 * 
 * - NOTE: this function assert if the totfile_range_t is not fully included
 *   in a single piece
 */
bt_prange_t	bt_prange_t::from_totfile_range(const file_range_t &totfile_range
						, const bt_mfile_t &bt_mfile)	throw()
{
	size_t		piece_len	= bt_mfile.piecelen();
	bt_prange_t	bt_prange;
	// sanity check - the totfile file_range_t MUST both be on the same piece_idx
	DBG_ASSERT( totfile_range.beg() / piece_len == totfile_range.end() / piece_len );
	// set each fields of the bt_prange_t
	bt_prange.piece_idx	( file_size_t(totfile_range.beg() / piece_len).to_size_t() );
	bt_prange.offset	( file_size_t(totfile_range.beg() % piece_len).to_size_t() );
	bt_prange.length	( totfile_range.len().to_size_t() );
	// return the resulting the bt_prange_t
	return bt_prange;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int bt_prange_t::compare(const bt_prange_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// compare the piece_idx
	if( piece_idx() < other.piece_idx() )	return -1;	
	if( piece_idx() > other.piece_idx() )	return +1;	
	// NOTE: here both have the same piece_idx

	// compare the offset
	if( offset() < other.offset() )		return -1;	
	if( offset() > other.offset() )		return +1;	
	// NOTE: here both have the same offset

	// compare the length
	if( length() < other.length() )		return -1;	
	if( length() > other.length() )		return +1;	
	// NOTE: here both have the same length

	// here both are equal
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bt_prange_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss        << "piece_idx="	<< piece_idx();
	oss << " " << "offset="		<< offset();
	oss << " " << "length="		<< length();
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;







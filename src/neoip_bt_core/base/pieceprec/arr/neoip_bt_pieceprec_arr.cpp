/*! \file
    \brief Definition of the \ref bt_pieceprec_arr_t

*/

/* system include */
/* local include */
#include "neoip_bt_pieceprec_arr.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set a range within this bt_pieceprec_arr_t
 */
bt_pieceprec_arr_t &	bt_pieceprec_arr_t::set_range(size_t idx_beg, size_t idx_end
			, const bt_pieceprec_t &prec_beg, const bt_pieceprec_t & prec_end)	throw()
{
	// sanity check - idx_beg and idx_end MUST be < that .size()
	DBG_ASSERT( idx_beg <  size() );
	DBG_ASSERT( idx_end <  size() );
	DBG_ASSERT( idx_beg <= idx_end);
	// init some variables
	double	prec_cur	= prec_beg.value();
	double	prec_delta	= double(prec_end.value()) - double(prec_beg.value());
	double	prec_inc	= prec_delta / double(idx_end- idx_beg);
	// go thru the whole range
	for(size_t pieceidx = idx_beg; pieceidx <= idx_end; pieceidx++){
		// set this pieceidx to the current precendence
		(*this)[pieceidx]	= (size_t)prec_cur;
		// update the precedence
		prec_cur		+= prec_inc; 
	}
	// return the object itself
	return *this;
}

/** \brief Set a range with all bt_pieceprec_t set to a given value
 * 
 * - just an helper on top on set_range()
 */
bt_pieceprec_arr_t &	bt_pieceprec_arr_t::set_range_equal(size_t idx_beg, size_t idx_end
							, const bt_pieceprec_t &pieceprec)	throw()
{
	return set_range(idx_beg, idx_end, pieceprec, pieceprec);
}

/** \brief Set a range with decreasing priority
 * 
 * - just an helper on top on set_range()
 */
bt_pieceprec_arr_t &	bt_pieceprec_arr_t::set_range_decrease(size_t idx_beg, size_t idx_end
							, const bt_pieceprec_t &pieceprec)	throw()
{
	// sanity check- piece_prec can not be below bt_pieceprec_t::LOWEST
	DBG_ASSERT( pieceprec.value() >= bt_pieceprec_t::LOWEST + (idx_end - idx_beg));
	return set_range(idx_beg, idx_end, pieceprec, pieceprec.value() - (idx_end - idx_beg));
}

NEOIP_NAMESPACE_END;







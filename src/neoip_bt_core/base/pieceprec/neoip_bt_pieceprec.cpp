/*! \file
    \brief Definition of the \ref bt_pieceprec_t

\par Brief Description
\ref bt_pieceprec_t counts the number of occurences of each piece.

*/

/* system include */
/* local include */
#include "neoip_bt_pieceprec.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_pieceprec_arr_t constant
const size_t	bt_pieceprec_t::NOTNEEDED	= 0;
const size_t	bt_pieceprec_t::LOWEST		= 1;
const size_t	bt_pieceprec_t::HIGHEST		= std::numeric_limits<size_t>::max();
const size_t	bt_pieceprec_t::DEFAULT		= std::numeric_limits<size_t>::max()/2;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare function - retuning result ala memcmp
 */
int	bt_pieceprec_t::compare(const bt_pieceprec_t &other)		const throw()
{
	// compare the piece_arr
	if( value() < other.value() )	return -1;
	if( value() > other.value() )	return +1;
	// NOTE: both are considered equal
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bt_pieceprec_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// build the string
	if( value() == NOTNEEDED )	oss << "notneeded";
	else if( value() == LOWEST )	oss << "lowest";
	else if( value() == HIGHEST )	oss << "highest";
	else if( value() == DEFAULT )	oss << "default";
	else				oss << value();
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_pieceprec_t
 * 
 * - support null bt_pieceprec_t
 */
serial_t& operator << (serial_t& serial, const bt_pieceprec_t &pieceprec)		throw()
{
	// serialize each field of the object
	serial << pieceprec.value();
	// return serial
	return serial;
}

/** \brief unserialze a bt_pieceprec_t
 * 
 * - support null bt_pieceprec_t
 */
serial_t& operator >> (serial_t & serial, bt_pieceprec_t &pieceprec)		throw(serial_except_t)
{
	size_t	prec_value;
	// reset the destination variable
	pieceprec	= bt_pieceprec_t();
	// unserialize the data
	serial >> prec_value;
	// set the returned variable
	pieceprec	= prec_value;
	// return serial
	return serial;
}
NEOIP_NAMESPACE_END;







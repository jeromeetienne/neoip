/*! \file
    \brief Definition of the \ref bitcount_t

\par Brief Description
\ref bitcount_t counts the number of occurences of bits. This is well coupled
with bitfield_t which maintains the state of bits.

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_bitcount.hpp"
#include "neoip_bitfield.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Build a bitcount_t
 */
bitcount_t::bitcount_t(const size_t &nb_elem)	throw()
{
	// set it to false by default
	count_arr.assign(nb_elem, 0);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     query function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return a double representing the coverage factor of all the bits
 * 
 * - the coverage_factor is "the number of time ALL bit are present"
 *   - if only half of the bit counter are non 0, the coverage factor is 0.5
 *   - then if every bit is counted 3 time, the coverage factor is 3.5
 *   - a coverage_factor of < 1.0 mean that some bit never have been counted
 *   - a coverage_factor of >= 1.0 mean that ALL bit have been counter at least once.
 */
double	bitcount_t::coverage_factor()	const throw()
{
	size_t	min_count	= std::numeric_limits<size_t>::max();
	// compute the min_count - aka the integer part of the global factor
	for(size_t i = 0; i < size(); i++)	min_count = std::min(min_count, count_arr[i]);
	// compute the number of elem more frequent than the min_count - aka the fraction part
	size_t	nb_more_count	= 0;
	for(size_t i = 0; i < size(); i++ ){
		if( count_arr[i] > min_count )	nb_more_count++;
	}
	// return the global_count
	return min_count + double(nb_more_count) / size();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief increment a single bit
 * 
 * - likely to be used when a PIECE_ISAVAIL is received
 */
bitcount_t&	bitcount_t::inc(size_t idx)				throw()
{
	// update the count_arr
	count_arr[idx]++;
	// return the object itself
	return *this;
}

/** \brief deccrement a single bit
 */
bitcount_t&	bitcount_t::dec(size_t idx)				throw()
{
	// sanity check - the count_arr[idx] MUST be > 0
	DBG_ASSERT( count_arr[idx] > 0 );	
	// update the count_arr
	count_arr[idx]--;
	// return the object itself
	return *this;
}

/** \brief Add a whole bitfield_t to this bitcount_t
 */
bitcount_t&	bitcount_t::inc(const bitfield_t &bitfield)	throw()
{
	// sanity check - both MUST have the same size
	DBG_ASSERT( size() == bitfield.size() );
	// go thru the whole bitfield
	for(size_t idx = 0; idx < size(); idx++){
		// if the bit of this idx is set in the bitfield, update the count_arr
		if( bitfield.get(idx) )	count_arr[idx]++;
	}
	// return the object itself
	return *this;
}

/** \brief Sub a whole bitfield_t to this bitcount_t
 */
bitcount_t&	bitcount_t::dec(const bitfield_t &bitfield)	throw()
{
	// sanity check - both MUST have the same size
	DBG_ASSERT( size() == bitfield.size() );
	// go thru the whole bitfield
	for(size_t idx = 0; idx < size(); idx++){
		// if the bit of this idx is set in the bitfield, update the count_arr
		if( bitfield.get(idx) ){
			DBG_ASSERT( count_arr[idx] > 0 );
			count_arr[idx]--;
		}
	}
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			arithmetic operator +/- with other bitcount_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a whole bitfield_t to this bitcount_t
 */
bitcount_t&	bitcount_t::operator +=(const bitcount_t &other)	throw()
{
	// sanity check - both MUST have the same size
	DBG_ASSERT( size() == other.size() );
	// go thru the whole bitfield
	for(size_t idx = 0; idx < size(); idx++){
		count_arr[idx]	+= other[idx];
	}
	// return the object itself
	return *this;
}

/** \brief Sub a whole bitfield_t to this bitcount_t
 */
bitcount_t&	bitcount_t::operator -=(const bitcount_t &other)	throw()
{
	// sanity check - both MUST have the same size
	DBG_ASSERT( size() == other.size() );
	// go thru the whole bitfield
	for(size_t idx = 0; idx < size(); idx++){
		DBG_ASSERT( count_arr[idx] >= other[idx] );
		count_arr[idx]	-= other[idx];
	}
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare function - retuning result ala memcmp
 */
int	bitcount_t::compare(const bitcount_t &other)		const throw()
{
	// sanity check - they both have the same parameters
	DBG_ASSERT( size() == other.size() );

	// compare the count_arr
	if( count_arr < other.count_arr )	return -1;
	if( count_arr > other.count_arr )	return +1;
	
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
std::string bitcount_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << "[";
	for(size_t i = 0; i < size(); i++ ){
		if( i != 0 )	oss << ", ";
		oss << count_arr[i];
	}
	oss << "]";
	// return the just built string
	return oss.str();
}


NEOIP_NAMESPACE_END;







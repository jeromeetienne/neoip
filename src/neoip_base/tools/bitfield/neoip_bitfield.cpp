/*! \file
    \brief Definition of the \ref bitfield_t

\par Note about Speed
Currently this use a std::vector<bool> i think it could be a lot faster and use less
memory if it was based on a array of uint64_t.
- all the comparisons could be done by bunch of 64bit instead of bit per bit
- all the boolean operation could be faster too

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_bitfield.hpp"
#include "neoip_math_shortcut.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Build a bitfield_t
 */
bitfield_t::bitfield_t(size_t nb_bit)	throw()
{
	// set it to false by default
	bitfield_arr.assign(nb_bit, false);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if all the bits are set
 */
bool	bitfield_t::is_all_set()	const throw()
{
	// sanity check - the bt_pbitfiled MUST NOT be null
	DBG_ASSERT( !is_null() );
	// go thru the whole bitfield_t
	for(size_t i = 0; i < size(); i++){
		// if this bit IS NOT set, return false
		if( !get(i) )	return false;
	}
	// if this point is reached, return true
	return true;
}

/** \brief Return true if any the bits are set
 */
bool	bitfield_t::is_any_set()	const throw()
{
	// sanity check - the bt_pbitfiled MUST NOT be null
	DBG_ASSERT( !is_null() );
	// go thru the whole bitfield_t
	for(size_t i = 0; i < size(); i++){
		// if this bit IS set, return true
		if( get(i) )	return true;
	}
	// if this point is reached, return false
	return false;
}

/** \brief Return the number of bit set
 */
size_t	bitfield_t::nb_set()	const throw()
{
	size_t	nb_bit	= 0;
	// sanity check - the bt_pbitfiled MUST NOT be null
	DBG_ASSERT( !is_null() );
	// go thru the whole bitfield_t
	for(size_t i = 0; i < size(); i++){
		// if this bit IS set, increment nb_bit
		if( get(i) )	nb_bit++;
	}
	// return the result
	return nb_bit;
}

/** \brief Return true if all the bits present in other are present in this bitfield_t too
 */
bool	bitfield_t::fully_contain(const bitfield_t &other)	const throw()
{
	// sanity check - they both have the same parameters
	DBG_ASSERT( size() == other.size() );
	// convert the datum_t to a bitfield_t bit by bit
	for(size_t i = 0; i < other.size(); i++){
		// if this bit is set in other but not in this, return false
		if( other.get(i) && !get(i) )	return false;
	}
	// if this point is reached, the other is fully contained
	return true;
}

/** \brief Start searching for the next bit set begining with the start_idx
 * 
 * @return the idx of the next bit set, or idx = bitfield_t.size() if none
 */
size_t	bitfield_t::next_set(size_t idx)	const throw()
{
	// if the idx is already >= size(), return it now
	if( idx >= size() )	return idx;
	// look for the next bit set
	for(; idx < size() && !get(idx); idx++);
	// return the result
	return idx;	
}

/** \brief Start searching for the next bit unset begining with the start_idx
 * 
 * @return the idx of the next bit set, or idx = bitfield_t.size() if none
 */
size_t	bitfield_t::next_unset(size_t idx)	const throw()
{
	// if the idx is already >= size(), return it now
	if( idx >= size() )	return idx;
	// look for the next bit unset
	for(; idx < size() && get(idx); idx++);
	// return the result
	return idx;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Reinitialize a bitfield_t with a special value
 */
bitfield_t &	bitfield_t::assign(size_t nb_bit, bool value)	throw()
{
	// set it to false by default
	bitfield_arr.assign(nb_bit, value);
	// return the object itself
	return *this;
}
	

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			datum_t conversion
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the bitfield_t into a datum_t
 * 
 * - NOTE: it doesnt contains the size of the bitfield_t
 */
datum_t		bitfield_t::to_datum()				const throw()
{
	datum_t		datum	= datum_t(to_datum_size());
	uint8_t*	p	= datum.uint8_ptr();
	// set the whole datum_t to 0
	memset(p, 0, datum.size());
	// convert the datum_t to a bitfield_t bit by bit
	for(size_t i = 0; i < size(); i++){
		if( get(i) )	p[i/8]	|= 0x80 >> (i%8);
	}
	// return the just built datum_t
	return datum;
}

/** \brief return the bitfield_t from a datum_t containing a bitfield_t of nb_bit
 * 
 * - if an error occurs, return a null bitfield_t
 */
bitfield_t	bitfield_t::from_datum(const datum_t &datum, size_t nb_bit)	throw()
{
	// init the bitfield
	bitfield_t	bitfield(nb_bit);
	// if the datum_t size is not the proper one
	if( datum.size() != bitfield.to_datum_size() )	return bitfield_t();
	// convert the datum_t to a bitfield_t bit by bit
	const uint8_t *	p	= datum.uint8_ptr();
	for(size_t i = 0; i < nb_bit; i++){
		if( p[i/8] & (0x80 >> (i%8)) )	bitfield.set(i);
	}
	// return the just built bitfield_t
	return bitfield;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     arithmetic operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Do a boolean OR between this bitfield_t and the other and store the result in this one
 */
bitfield_t &bitfield_t::operator |=(const bitfield_t &other)	throw()
{
	// sanity check - they both have the same parameters
	DBG_ASSERT( size() == other.size() );
	// go thru all the other bits
	for(size_t i = 0; i < other.size(); i++){
		// if this bit is set in other, set it to true in this one
		if( other[i] )	set(i, true);
	}
	// return the object itself
	return *this;
}

/** \brief Do a boolean AND between this bitfield_t and the other and store the result in this one
 */
bitfield_t &bitfield_t::operator &=(const bitfield_t &other)	throw()
{
	// sanity check - they both have the same parameters
	DBG_ASSERT( size() == other.size() );
	// go thru all the other bits
	for(size_t i = 0; i < other.size(); i++){
		// if this bit IS NOT set in other, set it to false in this one
		if( !other[i] )	set(i, false);
	}
	// return the object itself
	return *this;
}

/** \brief Do a boolean XOR between this bitfield_t and the other and store the result in this one
 */
bitfield_t &bitfield_t::operator ^=(const bitfield_t &other)	throw()
{
	// sanity check - they both have the same parameters
	DBG_ASSERT( size() == other.size() );
	// go thru all the other bits
	for(size_t i = 0; i < other.size(); i++){
		// if this bit IS set in other, invert it in this one
		if( other[i] )	set(i, !get(i));
	}
	// return the object itself
	return *this;
}

/** \brief Do a boolean NOT on this bitfield_t but DONT modify it
 */
bitfield_t	bitfield_t::operator ~ ()	const throw()
{
	bitfield_t	bitfield(size());
	// go thru all the other bits
	for(size_t i = 0; i < size(); i++){
		// invert this bit
		bitfield.set(i, !get(i));
	}
	// return the resulting bitfield_t
	return bitfield;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare function - retuning result ala memcmp
 */
int	bitfield_t::compare(const bitfield_t &other)		const throw()
{
	// sanity check - they both have the same parameters
	DBG_ASSERT( size() == other.size() );

	// compare the bitfield
	if( bitfield_arr < other.bitfield_arr )		return -1;
	if( bitfield_arr > other.bitfield_arr )		return +1;
	
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
std::string bitfield_t::to_canonical_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	for(size_t i = 0; i < size(); i++ )	oss << bitfield_arr[i];
	// return the just built string
	return oss.str();
}
/** \brief convert the object into a string
 */
std::string bitfield_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << "[";
	for(size_t i = 0; i < size(); i++ )	oss << bitfield_arr[i];
	oss << "]";
	// return the just built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          serialization
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bitfield_t
 */
serial_t& operator << (serial_t& serial, const bitfield_t &bitfield)		throw()
{
	size_t		nb_byte	= ceil_div(bitfield.size(), size_t(8));
	datum_t		datum	= datum_t(nb_byte);
	uint8_t*	p	= (uint8_t *)datum.get_ptr();
	// set the whole datum_t to 0
	memset(p, 0, datum.size());
	// convert the datum_t to a bitfield_t bit by bit
	for(size_t i = 0; i < bitfield.size(); i++){
		if( bitfield[i] )	p[i/8]	|= 0x80 >> (i%8);
	}	
	// append the datum_t data
	serial << bitfield.size();
	serial.append( datum.get_ptr(), datum.size() );
	// return serial
	return serial;
}

/** \brief unserialze a bitfield_t
 */
serial_t& operator >> (serial_t & serial, bitfield_t &bitfield)		throw(serial_except_t)
{
	size_t	nb_bit;
	// get the nunber of bit
	serial >> nb_bit;
	// init the bitfield
	bitfield	= bitfield_t( nb_bit );
	// compute the amount of byte which are supposed to follow
	size_t	nb_byte	= ceil_div(bitfield.size(), size_t(8));
	// if the serial is not long enought, throw an exception
	if( serial.get_len() < nb_byte )	nthrow_serial_plain("bitfield_t too short");
	// consume the bitfield in the serial_t
	datum_t		datum(serial.get_data(), nb_byte);
	serial.consume( nb_byte );
	// convert the datum_t to a bitfield_t bit by bit
	const uint8_t *	p	= (const uint8_t *)datum.get_ptr();
	for(size_t i = 0; i < nb_bit; i++){
		if( p[i/8] & (0x80 >> (i%8)) )	bitfield.set(i, true);
	}
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END;







/*! \file
    \brief Implementation of the datum0_t

\par Possible Improvement
- the serialization overhead could be reduced
  - currently the length of the datum0_t is stored in uint32_t to avoid limit
  - as most datum0_t are quite small and doesnt require that much
  - a possibility would be: 
    - if length < 255, store the length in 1-byte
    - if length < 65535, store a 255 in a byte, and then the length in 2-byte
    - else store 3 time 255 byte and then the length in 4-byte
    - it is a good solution as the addiotionnal overhead (up to 3 byte for datum
      >= 2^16) is made negligible as it is proportionnal to the size of the datum

*/

/* system include */
#include <iostream>
#include <iomanip>

/* local include */
#include "neoip_datum0.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief constructor for disctinct data
 */
datum0_t::datum0_t( const void *orig_data, int orig_len, datum0_flag_t flag )	throw()
{
	init( orig_data, orig_len, flag );
}

/** \brief constructor to reserve a amount of data without content
 */
datum0_t::datum0_t( int orig_len, datum0_flag_t flag )				throw()
{
	init( NULL, orig_len, flag );
}

/** \brief explicit constructor from a char * string
 */
datum0_t::datum0_t(const char *str)				throw()
{
	init( str, strlen(str), FLAG_DFL );	
}

/** \brief explicit constructor from a std::string
 */
datum0_t::datum0_t(const std::string &str)				throw()
{
	init( str.c_str(), str.size(), FLAG_DFL );	
}
/** \biref copy contructors
 */
datum0_t::datum0_t(const datum0_t & other)						throw()
{
	// clear the nocopy flag to ensure the new data get copied
	datum0_flag_t	tmp_flag = flag_set(other.flag_bitfield, datum0_t::NOCOPY, false);
	// init the local datum0_t
	init(other.data, other.len, tmp_flag );
}

/** \brief desctructor
 */
datum0_t::~datum0_t()								throw()
{
	free_data_buffer();
}

/** \brief free the data buffer if needed
 * 
 * - handle the nocopy and secure datum0_flag_t
 */
void	datum0_t::free_data_buffer()			throw()
{
	if( data == NULL || flag_get(flag_bitfield, datum0_t::NOCOPY) )	return;
	if( flag_get(flag_bitfield, datum0_t::SECMEM) )	nipmem_free_sec(data, len);
	else						nipmem_free(data);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   assignement and init
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

datum0_t	operator +  (const datum0_t &datum_a, const datum0_t &datum_b)	throw()
{
	datum0_flag_t	tmp_flag = datum0_t::FLAG_DFL;
	size_t		new_len	= datum_a.get_len() + datum_b.get_len();
	// if any of the 2 source is datum0_t:SECMEM, the copy is too
	if( datum0_t::flag_get(datum_a.flag_bitfield, datum0_t::SECMEM) 
			|| datum0_t::flag_get(datum_b.flag_bitfield, datum0_t::SECMEM) ){
		tmp_flag = datum0_t::flag_set(tmp_flag, datum0_t::SECMEM, true);
	}
	// init the new datum
	datum0_t	datum(new_len, tmp_flag);
	// copy the datum_a data into the new datum
	memcpy( datum.get_data(), datum_a.get_data(), datum_a.get_len() );
	// copy the datum_b data into the new datum	
	memcpy( (uint8_t *)datum.get_data() + datum_a.get_len(), datum_b.get_data(), datum_b.get_len() );
	// logging to debug
	KLOG_DBG("a=" << datum_a << " b=" << datum_b << " a+b=" << datum );
	return datum;
}

/** \brief overload the = operator
 */
datum0_t &datum0_t::operator=( const datum0_t & other )
{
	// if auto-assignment, do nothing
	if( this == &other )	return *this;

	// free the local data buffer
	free_data_buffer();

	// clear the nocopy flag to ensure the new data get copied
	datum0_flag_t	tmp_flag = flag_set(other.flag_bitfield, datum0_t::NOCOPY, false);
	// init the local datum0_t
	init(other.data, other.len, tmp_flag );
	return *this;
}

/** \brief a initiatlization function
 */
void datum0_t::init( const void *orig_data, int orig_len, datum0_flag_t flag )	throw()
{
	len		= orig_len;
	flag_bitfield	= flag;

	if( len == 0 ){
		data = NULL;
		return;
	}

	// allocate the memory
	if( flag_get(flag_bitfield, datum0_t::NOCOPY) )		data	= (void *)orig_data;
	else if( flag_get(flag_bitfield, datum0_t::SECMEM) )	data	= nipmem_malloc_sec(len);
	else							data	= nipmem_malloc(len);
		
	// copy the data if needed
	if( flag_get(flag_bitfield, datum0_t::NOCOPY) == false ){
		if( orig_data )	memcpy( data, orig_data, len );
		else		memset( data, 0, len );
	}
}

/** \brief return a given range of data in a datum0_t
 * 
 * - the whole range is assumed to be in the original datum0_t
 * - the datum0_t::SECMEM is inherited in the produced datum0_t
 */
datum0_t	datum0_t::get_range(size_t range_offset, size_t range_len, datum0_flag_t range_flag)	const throw()
{
	// sanity check - the whole range must be available
	DBG_ASSERT( range_offset + range_len <= get_len() );
	// if the original datum0_t is secure, the range will be too
	if( flag_get(flag_bitfield, datum0_t::SECMEM) )
		range_flag = flag_set(flag_bitfield, datum0_t::SECMEM, true);
	// return the produced range
	return datum0_t( (uint8_t *)get_data() + range_offset, range_len, range_flag );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   static function to get/set flag (private functions)
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return the value of a flag_bitfield
 */
bool	datum0_t::flag_get(datum0_flag_t flag_bitfield, datum0_flag_t flag_value)		throw()
{
	return	flag_bitfield & flag_value;
}

/** \brief set a flag in a flag_bitfield to the on value and return the result
 */
datum0_flag_t	datum0_t::flag_set(datum0_flag_t flag_bitfield, datum0_flag_t flag_value, bool on)	throw()
{
	if( on )	flag_bitfield	|= flag_value;
	else		flag_bitfield	&= ~flag_value;
	return flag_bitfield;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      COMPARISON
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Compare 2 objects ala memcmp
 */
int	datum0_t::compare(const datum0_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// handle them by the length - if both are not of the same length, the shorter is less than
	if( get_len() < other.get_len() )	return -1;
	if( get_len() > other.get_len() )	return +1;
	// NOTE: here both have the same length

	// if both length are equal, compare the data
	return memcmp( get_data(), other.get_data(), get_len() );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        OSTREAM redirection (mainly debug)
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief redirect the ostream << operator
 */
std::ostream & operator << (std::ostream & os, const datum0_t &datum )	throw()
{
	uint8_t	*p = (uint8_t *)datum.get_data();
	os << "len=" << datum.get_len();
	if( datum0_t::flag_get(datum.flag_bitfield, datum0_t::SECMEM) )	os << " (secure)";
	if( datum0_t::flag_get(datum.flag_bitfield, datum0_t::NOCOPY) )	os << " (nocopy)";
	os << " data=";
	if( datum.get_len() == 0 )	return os;
	os << "\n";
	for( size_t offset = 0 ; offset < datum.get_len() ; offset += 16 ){
		// put the line offset
		os << std::hex << std::setfill('0') << std::setw(4) << offset << ":";
		// put the data in hexa
		for( int i = 0 ; i < 16 ; i++ ){
			if( offset+i < datum.get_len() ){
				os << " " << std::hex << std::setfill('0') << std::setw(2) << (int)p[offset+i];
			}else{
				os << "   ";
			}
		}
		os << "\t| ";
		// put the data in printable ascii
		for( int i = 0 ; i < 16 ; i++ ){
			if( offset+i < datum.get_len() ){
				if( isprint(p[offset+i]) )	os << p[offset+i];
				else				os << ".";
			}else{
				os << " ";
			}
		}
		os << " |";
		if( offset + 16 < datum.get_len() )
			os << "\n";
	}
	return os;
}

/** \brief convert the object to a string
 */
std::string	datum0_t::to_string()	const throw()
{
	std::ostringstream      oss;
	uint8_t	*		p	= (uint8_t *)get_data();
	// if the datum is empty, return a null string
	if( get_len() == 0 )	return oss.str();
	// else return the datum in a big "0xfffff" form
	oss << "0x";
	for( size_t offset = 0 ; offset < get_len() ; offset++ )
		oss << std::hex << std::setfill('0') << std::setw(2) << (int)p[offset];
	// return the built string
	return oss.str();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial datum0_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a \ref datum0_t
 */
serial_t& operator << (serial_t& serial, const datum0_t &datum)		throw()
{
	// serialize the length
	serial	<< (uint32_t)datum.get_len();
	// serialize the data
	serial.append( datum.get_data(), datum.get_len() );
	return serial;
}

/** \brief unserialize a \ref datum0_t
 */
serial_t& operator >> (serial_t& serial, datum0_t &datum) 		throw(serial_except_t)
{
	// get the length in the payload
	uint32_t	len;
	serial		>> len;
	// check the length
	if( serial.get_len() < len )
		throw serial_except_t("datum0_t Payload Too Short");
	// if the original datum is secured, keep the unserialized one secure
	datum0_flag_t	tmp_flag = datum0_t::FLAG_DFL;
	if( datum0_t::flag_get(datum.flag_bitfield, datum0_t::SECMEM) )
		tmp_flag = datum0_t::flag_set(datum.flag_bitfield, datum0_t::SECMEM, true);
	// build the datum
	datum = datum0_t( serial.get_data(), len, tmp_flag );
	// consume the buffer
	serial.consume( len );
	return serial;
}


NEOIP_NAMESPACE_END

/*! \file
    \brief Implementation of the serial0_t
*/

/* system include */
#include <stdint.h>
#include <netinet/in.h>

/* local include */
#include "neoip_serial0.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief consructor of the class
 */
serial0_t::serial0_t( int reserved_size )			throw()
{
	// init the buffer
	buf	= g_byte_array_sized_new(2*1024);
}

/** \brief consructor of the class
 */
serial0_t::serial0_t( const void *data, int len )		throw()
{
	// init the buffer
	buf	= g_byte_array_sized_new(len);
	// copy the buffer
	buf	= g_byte_array_append( buf, (const guint8 *)data, len );
}

/** \brief destructor of the class
 */
serial0_t::~serial0_t()					throw()
{
	// sanity check
	DBG_ASSERT( buf );
	// free the buffer
	g_byte_array_free( buf, 1 );
}

/** \brief append a bunch of data into the serial0_t
 */
void	serial0_t::append( const void *buf_data, int buf_len )	throw()
{
	buf	= g_byte_array_append( buf, (const guint8 *)buf_data, buf_len );
	DBG_ASSERT( buf );
}

/** \brief consume a bunch of data into the serial0_t
 */
void	serial0_t::consume( size_t length )			throw()
{	
	// remove the range iif len > 0 (because g_byte_array refuse to remove index 0 and len 0 if 
	// the array doesnt contains anything
	if( length == 0 )	return;
	buf	= g_byte_array_remove_range( buf, 0, length);
	DBG_ASSERT( buf );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial bool (encoded as a uint8_t)
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief overload the << operator
 */
serial0_t& operator << (serial0_t& serial, const bool &val)	throw()
{
	uint8_t	payl = val ? 1 : 0;
	serial.append( &payl, sizeof(payl) );
	return serial;
}

/** \brief overload the >> operator
 */
serial0_t& operator >> ( serial0_t& serial, bool &val )	 throw(serial0_except_t)
{
	bool	tmp;
	// check the payload length
	if(serial.get_len() < (ssize_t)sizeof(tmp))
		nthrow_serial0_plain("bool Payload Too Short");	
	// read the payload
	tmp = *((uint8_t *)serial.get_data());
	// consume the buffer
	serial.consume( sizeof(tmp) );
	// convert tmp into val
	val = tmp ? true : false;
	// return the serial0_t itself
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial uint8_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/** \brief overload the << operator
 */
serial0_t& operator << ( serial0_t& serial, const uint8_t &val )	throw()
{
	uint8_t	payl = val;
	serial.append( &payl, sizeof(payl) );
	return serial;
}

/** \brief overload the >> operator
 */
serial0_t& operator >> ( serial0_t& serial, uint8_t &val )	 throw(serial0_except_t)
{
	// check the payload length
	if(serial.get_len() < (ssize_t)sizeof(val))
		nthrow_serial0_plain("uint8 Payload Too Short");	
	// read the payload
	val = *((uint8_t *)serial.get_data());
	// consume the buffer
	serial.consume( sizeof(val) );
	return serial;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial uint16_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/** \brief overload the << operator
 */
serial0_t& operator << ( serial0_t& serial, const uint16_t &val )	throw()
{
	uint16_t	payl = htons(val);
	serial.append( &payl, sizeof(payl) );
	return serial;
}

/** \brief overload the >> operator
 */
serial0_t& operator >> ( serial0_t& serial, uint16_t &val ) 	throw(serial0_except_t)
{
	// check the payload length
	if(serial.get_len() < (ssize_t)sizeof(val))
		nthrow_serial0_plain("uint16 Payload Too Short");	
	// read the payload
	val = ntohs(*(uint16_t *)serial.get_data());
	// consume the buffer
	serial.consume( sizeof(val) );
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial uint32_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/** \brief overload the << operator
 */
serial0_t& operator << ( serial0_t& serial, const uint32_t &val )	throw()
{
	uint32_t	payl = htonl(val);
	serial.append( &payl, sizeof(payl) );
	return serial;
}

/** \brief overload the >> operator
 */
serial0_t& operator >> ( serial0_t& serial, uint32_t &val )	 throw(serial0_except_t)
{
	// check the payload length
	if(serial.get_len() < (ssize_t)sizeof(val))
		nthrow_serial0_plain("uint32 Payload Too Short");	
	// read the payload
	val = ntohl(*(uint32_t *)serial.get_data() );
	// consume the buffer
	serial.consume( sizeof(val) );
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial uint64_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/** \brief overload the << operator
 */
serial0_t& operator << ( serial0_t& serial, const uint64_t &val )		throw()
{
	uint64_t	payl = GUINT64_TO_BE(val);
	serial.append( &payl, sizeof(payl) );
	return serial;
}

/** \brief overload the >> operator
 */
serial0_t& operator >> ( serial0_t& serial, uint64_t &val )		throw(serial0_except_t)
{
	// check the payload length
	if( serial.get_len() < (ssize_t)sizeof(val) )
		nthrow_serial0_plain("uint64 Payload Too Short");	
	// read the payload
	val = GUINT64_FROM_BE( *(uint64_t *)serial.get_data());
	// consume the buffer
	serial.consume( sizeof(val) );
	return serial;
}
NEOIP_NAMESPACE_END


/*! \file
    \brief Definition of the compress_t

\par Possible improvements
-# support the power of compression
  - the power of a compression algorithm can be tuned according to a tradeoff
    size of the compressed data vs cpu/memory usage
  - to add an option in the compress_type_t string, e.g. bzip/100 would 
    allow to use a bzip algorithm to the best of its capacity.
  - In the current implementation, the default value is used
-# support compression history between compressed datagram
  - sharing compression history will result in smaller compressed data
  - it require that all compressed datagrams are garanteed to arrives
    to the uncompressor and to arrive in order. so for \ref neoip-socket, 
    the \ref neoip-socket-type must be SEQPACKET or STREAM.
  - security hint: share compression history only inside a single connection
    and not accross several connections. or it may cause informations leaks
    - see tls compression rfc2943 section 2.2 http://www.faqs.org/rfcs/rfc3943.html
  - In the current implementation, the history isnt shared at all.

\par TODO
- the max_len in ::compress() and ::uncompress() can't have any default because
  it is a general compression lib.
  - TODO ok outside of this layer, what is the value i put for this max size ?

*/


/* system include */
/* local include */
#include "neoip_compress.hpp"
#include "neoip_compress_layer.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Constructor from string
 */
compress_t::compress_t(const compress_type_t &compress_type) throw()
{
	// zeroing() the object before init
	zeroing();
	
	// init the api from the factory
	compress_vapi	= compress_factory->create(compress_type);
	if( compress_vapi == NULL )	return;
	// copy the type
	this->compress_type	= compress_type;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   COMPRESS/UNCOMPRESS function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief compress the \ref datum_t and return the compressed result
 * 
 * @param datum		the data \ref datum_t to compress
 * @param max_len	the maximum length of the result
 * @return		a \ref datum_t containing the compressed data. the \ref datum_t
 * 			is null if the result is larger than \ref max_len.
 */
datum_t	compress_t::compress(const datum_t &datum, size_t max_len)	throw()
{
	// sanity check - if the compress_t is null, return a null datum_t
	if( is_null() )	return datum_t();
	// call the compress function of this compress_vapi
	return compress_vapi->compress(datum, max_len);
}

/** \brief uncompress the \ref datum_t and return the uncompressed result
 * 
 * @param datum		the data \ref datum_t to uncompress
 * @param max_len	the maximum length of the result
 * @return		a \ref datum_t containing the uncompressed data. the \ref datum_t
 * 			is null if the result is larger than \ref max_len.
 */
datum_t	compress_t::uncompress(const datum_t &datum, size_t max_len)	throw()
{
	// sanity check - if the compress_t is null, return a null datum_t
	if( is_null() )	return datum_t();
	// call the uncompress function of this compress_vapi
	return compress_vapi->uncompress(datum, max_len);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief initial zeroing of the object (no free is made)
 */
void	compress_t::zeroing()	throw()
{
	compress_vapi = NULL;
}
	
/** \brief nullify the object (aka free it if needed and after this function, is_null() == true)
 */
void	compress_t::nullify()		throw()
{ 
	if( is_null() )	return;
	
	compress_type= compress_type_t();
	compress_vapi->destroy();
	compress_vapi = NULL;
}

/** \brief copy a object to the local one (works even if the local one is non null)
 */
void	compress_t::copy(const compress_t &other) throw()
{
	nullify();
	if( other.is_null() )	return;
	
	compress_type= other.compress_type;
	compress_vapi = other.compress_vapi->clone();
}

/** \brief return true if the object is null
 */
bool	compress_t::is_null()		const throw()
{
	if( compress_type.is_null() )	return true;
	return false;
}

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int compress_t::compare( const compress_t & other )  const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// Handle the case where they are not both of the same type
	if( compress_type < other.compress_type)	return -1;
	if( compress_type > other.compress_type)	return +1;
	// NOTE: here both of the same type

	// here both are considered equal
	return 0;
}

/** \brief convert the object into a string
 */
std::string compress_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return compress_type.to_string();
}

NEOIP_NAMESPACE_END


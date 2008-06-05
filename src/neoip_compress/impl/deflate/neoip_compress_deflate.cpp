/*! \file
    \brief definition of the \ref compress_deflate_t

- TODO the implementation cause the zlib to leak memory
  - i think it is me using it badly. but currently i dont have the time to fix it
  - note: bzip and zlib compression works ok
    - but gzip and deflat leaks memory
    - a common point is the fact to use 'weird' stuff in zlib

*/

/* system include */
#include <zlib.h>
/* local include */
#include "neoip_compress_deflate.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

FACTORY_PRODUCT_DEFINITION(compress_vapi_t, compress_deflate_t);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          COMPRESS/UNCOMPRESS function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief compress the \ref datum_t and return the compressed result
 * 
 * - the compression is done in zlib which is just a container for deflate
 *   Then the zlib header/trailler are removed
 * 
 * @param datum		the data \ref datum_t to compress
 * @param max_len	the maximum length of the result
 * @return		a \ref datum_t containing the compressed data. the \ref datum_t
 * 			is null if the result is larger than \ref max_len.
 */
datum_t	compress_deflate_t::compress(const datum_t &datum, size_t max_len)	throw()
{
	// some zlib file format headers constant
	// - see rfc1950.2.2 for details about zlib header format
	size_t	zlib_header_len		= 2;	// CMF+FLG
	size_t	zlib_trailer_len	= 4;	// ALDER32
	size_t	zlib_overhead_len	= zlib_header_len + zlib_trailer_len;
	// log to debug
	KLOG_DBG("enter");
#if 0	// TODO to fix the memory leak
	DBG_ASSERT( 0 );
#endif
	// allocate the output buffer
	uLongf	outlen	= max_len + zlib_overhead_len;
	uint8_t	*outbuf = (uint8_t *)nipmem_alloca(outlen);
	// try to compress the data and put them in the output buffer
	int	err	= ::compress(outbuf, &outlen, (const uint8_t *)datum.get_data(), datum.get_len());
	// if the compression failed, return a NULL datum_t()
	if( err != Z_OK )	return	datum_t();

	// update the pointer and length to remove the zlib header
	outbuf	+= zlib_header_len;
	outlen	-= zlib_overhead_len;
	
	// copy the compressed data into a datum_t
	// - with some computation to remove the zlib header
	return datum_t(outbuf, outlen);
}

/** \brief uncompress the \ref datum_t and return the uncompressed result
 * 
 * @param datum		the data \ref datum_t to uncompress
 * @param max_len	the maximum length of the result
 * @return		a \ref datum_t containing the uncompressed data. the \ref datum_t
 * 			is null if the result is larger than \ref max_len.
 */
datum_t	compress_deflate_t::uncompress(const datum_t &datum, size_t max_len)	throw()
{
	int		ret;
	z_stream	stream;
	// log to debug
	KLOG_DBG("enter");
	// allocate the output buffer
	uint8_t	*outbuf = (uint8_t *)nipmem_alloca(max_len);
#if 1	// TODO to fix the memory leak
	DBG_ASSERT( 0 );
#endif
	// init deflate state
	stream.zalloc	= Z_NULL;
	stream.zfree	= Z_NULL;
	stream.opaque	= Z_NULL;
	stream.avail_in	= 0;
	stream.next_in	= Z_NULL;
	// NOTE: -15 is a important magic number - it asks the zlib library not to look at 
	//       zlib header
	// from zlib.h "windowBits can also be -8..-15 for raw inflate. In this case, -windowBits
	//              determines the window size. inflate() will then process raw deflate data,
	//              not looking for a zlib or gzip header, not generating a check value, and not
	//              looking for any check values for comparison at the end of the stream."
	ret		= inflateInit2(&stream, -15);
	if(ret != Z_OK)		return datum_t();

	// do the compression
	stream.next_in	= (uint8_t *)datum.get_data();
	stream.avail_in	= datum.get_len();
	stream.next_out	= outbuf;
	stream.avail_out= max_len;
	ret		= inflate(&stream, Z_FINISH);	// no bad return value
	
	// deallocate the deflate state
	deflateEnd(&stream);
	
	// if the compression failed, return a null datum_t
	if(ret != Z_STREAM_END)		return datum_t();

	// return the result
	return datum_t(outbuf, max_len - stream.avail_out);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			factory_domain_insert
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Populate the compress_factory factory with all the compress_type_t::DEFLATE class
 */
void	compress_deflate_t::factory_domain_insert()				throw()
{
	// insert all the factory_product_t for compress_type_t::DEFLATE
	FACTORY_PRODUCT_INSERT(compress_factory, compress_type_t, compress_vapi_t
				, strtype_compress_type_t::DEFLATE, compress_deflate_t);
}
NEOIP_NAMESPACE_END





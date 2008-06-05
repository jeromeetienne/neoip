/*! \file
    \brief definition of the \ref compress_gzip_t

- TODO the implementation cause the zlib to leak memory
  - i think it is me using it badly. but currently i dont have the time to fix it
  - note: bzip and zlib compression works ok
    - but gzip and deflat leaks memory
    - a common point is the fact to use 'weird' stuff in zlib

*/

/* system include */
#include <zlib.h>
/* local include */
#include "neoip_compress_gzip.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

FACTORY_PRODUCT_DEFINITION(compress_vapi_t, compress_gzip_t);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          COMPRESS/UNCOMPRESS function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief compress the \ref datum_t and return the compressed result
 * 
 * @param datum		the data \ref datum_t to compress
 * @param max_len	the maximum length of the result
 * @return		a \ref datum_t containing the compressed data. the \ref datum_t
 * 			is null if the result is larger than \ref max_len.
 */
datum_t	compress_gzip_t::compress(const datum_t &datum, size_t max_len)	throw()
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
	int	err	= ::compress(outbuf, &outlen, datum.uint8_ptr(), datum.length());
	// if the compression failed, return a NULL datum_t()
	if( err != Z_OK )	return	datum_t();

	// update the pointer and length to remove the zlib header
	outbuf	+= zlib_header_len;
	outlen	-= zlib_overhead_len;
		
	// NOTE: at this point, (outbuf,outlen) contains the datum compressed by deflate
	
	// add the gzip header/trailer and return the result
	return build_gzip_header()				// put the gzip header
		+ datum_t(outbuf, outlen, datum_t::NOCOPY)	// put the datum compressed by deflate
		+ build_gzip_trailer(datum);			// put the gzip trailer
}

/** \brief uncompress the \ref datum_t and return the uncompressed result
 * 
 * @param datum		the data \ref datum_t to uncompress
 * @param max_len	the maximum length of the result
 * @return		a \ref datum_t containing the uncompressed data. the \ref datum_t
 * 			is null if the result is larger than \ref max_len.
 */
datum_t	compress_gzip_t::uncompress(const datum_t &datum, size_t max_len)	throw()
{
	size_t	gzip_header_len		= cpu_gzip_header_len(datum);
	size_t	gzip_trailer_len	= 8;	// CRC32/32bit + ISIZE/32bit
	size_t	gzip_overhead_len	= gzip_header_len + gzip_trailer_len;
	int	ret;
	// log to debug
	KLOG_DBG("enter");
#if 0	// TODO to fix the memory leak
	DBG_ASSERT( 0 );
#endif
	
	// if the compressed datum is not large enouth contains the header/trailer, report an error
	if( datum.get_len() < gzip_overhead_len )	return datum_t();
		
	// allocate the output buffer
	uint8_t*outbuf	= (uint8_t *)nipmem_alloca(max_len);

// decompress the deflate data
	// init deflate state
	z_stream	stream;
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

	// do the decompression of the deflate part (in the gzip container)
	stream.next_in	= (uint8_t *)datum.get_data() + gzip_header_len;
	stream.avail_in	= datum.get_len() - gzip_overhead_len;
	stream.next_out	= outbuf;
	stream.avail_out= max_len;
	ret		= inflate(&stream, Z_FINISH);	// no bad return value
	// deallocate the deflate state
	deflateEnd(&stream);
	// if the compression failed, return a null datum_t
	if(ret != Z_STREAM_END)		return datum_t();

	// compute the size of the uncompressed result
	size_t	outlen	= max_len - stream.avail_out;
	
// verify the crc32
	// get the incoming crc from the datum
	uint8_t	*	p	= (uint8_t *)datum.get_data() + datum.get_len() - gzip_trailer_len;
	uint32_t	rcv_crc = (p[0] <<  0) + (p[1] <<  8) + (p[2] << 16) + (p[3] << 24);
	// compute the crc of the uncompressed datum_t
	uint32_t	cpu_crc = crc32(0L, (const Bytef*)outbuf, (uInt)outlen);
	// if the received crc and the computed one doesnt match, return an error
	if( cpu_crc != rcv_crc ){
		KLOG_ERR("Tried to decompress a gziped buffer but the crc32 is invalid!");
		return datum_t();
	}

	// copy the decompressed data into a datum_t
	return datum_t(outbuf, outlen);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return the length of the gzip header in the compressed datum
 */
size_t	compress_gzip_t::cpu_gzip_header_len(const datum_t &compressed_datum)	const throw()
{
	// TODO to do something which support option
	// - see rfc1951.2.3.1 for details about the format
	return 10;
}

/** \brief Return a standard gzip header with no option
 */
datum_t	compress_gzip_t::build_gzip_header()	const throw()
{

	/* add the gzip header
	 * - with those header/footer the result is directly uncompressable via gunzip
	 * - see rfc1951.2.3.1 for details about the format
	 */
 	char	header[] = {	0x1f,	// ID1 - magic number to identify the file
				0x8b, 	// ID2 - second magic number
				0x08,	// CM - compression method - 0x08 is deflate
				0x00,	// FLG - flag - none are set here
				0x00,	// MTIME - modification time of the original file (zeroed here)
				0x00,	//       - it is 4byte long
				0x00,
				0x00,
				0x02,	// XFL - extra flag - set to 2 for 'compressor used 
					// maximum compression, slowest algorithm'
				0xFF	// OS - operating system - 0xFF = unknown
				};

	// return a datum of it
	return datum_t(header, sizeof(header));
}

/** \brief Return a standard gzip header with no option
 */
datum_t	compress_gzip_t::build_gzip_trailer(const datum_t &uncompressed_datum)	const throw()
{
	// compute the trailer
	serial_t	trailer;
	// compute the crc of the uncompressed datum_t
	uLong crc = crc32(0L, (const Bytef*)uncompressed_datum.get_data()
					, (uInt)uncompressed_datum.get_len());
	trailer << (uint8_t) ((crc >>  0) & 0xff);
	trailer << (uint8_t) ((crc >>  8) & 0xff);
	trailer << (uint8_t) ((crc >> 16) & 0xff);
	trailer << (uint8_t) ((crc >> 24) & 0xff);
	// put the length of the uncompressed datum_t
	size_t	uncompressed_len = uncompressed_datum.get_len();
	trailer << (uint8_t) ((uncompressed_len >>  0) & 0xff);
	trailer << (uint8_t) ((uncompressed_len >>  8) & 0xff);
	trailer << (uint8_t) ((uncompressed_len >> 16) & 0xff);
	trailer << (uint8_t) ((uncompressed_len >> 24) & 0xff);

	// return the result
	return datum_t( trailer.get_data(), trailer.get_len());
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			factory_domain_insert
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Populate the compress_factory factory with all the compress_type_t::GZIP class
 */
void	compress_gzip_t::factory_domain_insert()				throw()
{
	// insert all the factory_product_t for compress_type_t::GZIP
	FACTORY_PRODUCT_INSERT(compress_factory, compress_type_t, compress_vapi_t
				, strtype_compress_type_t::GZIP, compress_gzip_t);
}

NEOIP_NAMESPACE_END








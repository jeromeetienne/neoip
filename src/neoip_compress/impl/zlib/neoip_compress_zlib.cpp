/*! \file
    \brief definition of the \ref compress_zlib_t
*/

/* system include */
#include <zlib.h>
/* local include */
#include "neoip_compress_zlib.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

FACTORY_PRODUCT_DEFINITION(compress_vapi_t, compress_zlib_t);

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
 * 			is zlib if the result is larger than \ref max_len.
 */
datum_t	compress_zlib_t::compress(const datum_t &datum, size_t max_len)	throw()
{
	uLongf	outlen	= max_len;
	// log to debug
	KLOG_DBG("enter");
	// allocate the output buffer
	uint8_t	*outbuf = (uint8_t *)nipmem_alloca(max_len);
	// try to compress the data and put them in the output buffer
	int	err	= ::compress(outbuf, &outlen, (const uint8_t *)datum.void_ptr(), datum.length());
	// if the compression failed, return a NULL datum_t()
	if( err != Z_OK )	return	datum_t();
	// copy the compressed data into a datum_t
	return datum_t(outbuf, outlen);
}

/** \brief uncompress the \ref datum_t and return the uncompressed result
 * 
 * @param datum		the data \ref datum_t to uncompress
 * @param max_len	the maximum length of the result
 * @return		a \ref datum_t containing the uncompressed data. the \ref datum_t
 * 			is zlib if the result is larger than \ref max_len.
 */
datum_t	compress_zlib_t::uncompress(const datum_t &datum, size_t max_len)	throw()
{
	uLongf	outlen	= max_len;
	// log to debug
	KLOG_DBG("enter");
	// allocate the output buffer
	uint8_t	*outbuf = (uint8_t *)nipmem_alloca(max_len);
	// try to uncompress the data and put them in the output buffer
	int	err	= ::uncompress(outbuf, &outlen,(const uint8_t*)datum.get_data(), datum.get_len());
	// if the decompression failed, return a NULL datum_t()
	if( err != Z_OK )	return	datum_t();
	// copy the decompressed data into a datum_t
	return datum_t(outbuf, outlen);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			factory_domain_insert
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Populate the compress_factory factory with all the compress_type_t::ZLIB class
 */
void	compress_zlib_t::factory_domain_insert()				throw()
{
	// insert all the factory_product_t for compress_type_t::ZLIB
	FACTORY_PRODUCT_INSERT(compress_factory, compress_type_t, compress_vapi_t
				, strtype_compress_type_t::ZLIB, compress_zlib_t);
}


NEOIP_NAMESPACE_END





/*! \file
    \brief definition of the \ref compress_null_t
*/

/* system include */
#include <zlib.h>
/* local include */
#include "neoip_compress_null.hpp"

NEOIP_NAMESPACE_BEGIN

FACTORY_PRODUCT_DEFINITION(compress_vapi_t, compress_null_t);


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
datum_t	compress_null_t::compress(const datum_t &datum, size_t max_len)	throw()
{
	if( datum.get_len() > max_len )	return datum_t();
	return datum;
}

/** \brief uncompress the \ref datum_t and return the uncompressed result
 * 
 * @param datum		the data \ref datum_t to uncompress
 * @param max_len	the maximum length of the result
 * @return		a \ref datum_t containing the uncompressed data. the \ref datum_t
 * 			is null if the result is larger than \ref max_len.
 */
datum_t	compress_null_t::uncompress(const datum_t &datum, size_t max_len)	throw()
{
	if( datum.get_len() > max_len )	return datum_t();
	return datum;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			factory_domain_insert
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Populate the compress_factory factory with all the compress_type_t::NULL_COMP class
 */
void	compress_null_t::factory_domain_insert()				throw()
{
	// insert all the factory_product_t for compress_type_t::NULL_COMP
	FACTORY_PRODUCT_INSERT(compress_factory, compress_type_t, compress_vapi_t
				, strtype_compress_type_t::NULL_COMP, compress_null_t);
}

NEOIP_NAMESPACE_END





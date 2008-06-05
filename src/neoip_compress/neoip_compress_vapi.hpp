/*! \file
    \brief Header of the \ref compress_vapi_t
*/


#ifndef __NEOIP_COMPRESS_VAPI_HPP__ 
#define __NEOIP_COMPRESS_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_obj_factory.hpp"
#include "neoip_compress_type.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Definition of the virtual API for the compress_t
 */
class compress_vapi_t {
public:
	/** \brief compress the \ref datum_t and return the compressed result
	 * 
	 * @param datum		the data \ref datum_t to compress
	 * @param max_len	the maximum length of the result
	 * @return		a \ref datum_t containing the compressed data. the \ref datum_t
	 * 			is null if the result is larger than \ref max_len.
	 */
	virtual datum_t	compress(const datum_t &datum, size_t max_len)	throw() = 0;

	/** \brief uncompress the \ref datum_t and return the uncompressed result
	 * 
	 * @param datum		the data \ref datum_t to uncompress
	 * @param max_len	the maximum length of the result
	 * @return		a \ref datum_t containing the uncompressed data. the \ref datum_t
	 * 			is null if the result is larger than \ref max_len.
	 */
 	virtual datum_t	uncompress(const datum_t &datum, size_t max_len)	throw() = 0;
 
	// declaration for the factory
	FACTORY_BASE_CLASS_DECLARATION(compress_vapi_t);
	//! virtual destructor
	virtual ~compress_vapi_t() {};
};


// declaration of the factory
FACTORY_PLANT_DECLARATION(compress_factory, compress_type_t, compress_vapi_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_COMPRESS_VAPI_HPP__  */




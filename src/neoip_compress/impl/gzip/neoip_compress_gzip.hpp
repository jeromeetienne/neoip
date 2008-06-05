/*! \file
    \brief Declaration of the compress_gzip_t
*/


#ifndef __NEOIP_COMPRESS_GZIP_HPP__ 
#define __NEOIP_COMPRESS_GZIP_HPP__ 
/* system include */
/* local include */
#include "neoip_compress_vapi.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief gzip implementation of the compress_t API
 */
class compress_gzip_t : public compress_vapi_t {
private:
	datum_t	build_gzip_header()					const throw();
	datum_t	build_gzip_trailer(const datum_t &uncompressed_datum)	const throw();
	size_t	cpu_gzip_header_len(const datum_t &compressed_datum)	const throw();
	
public:
	// list of inherited virtual function from \ref compress_vapi_t
	datum_t	compress(const datum_t &datum, size_t max_len)		throw();
	datum_t	uncompress(const datum_t &datum, size_t max_len)	throw();
	
	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(compress_vapi_t, compress_gzip_t);

	static void	factory_domain_insert()				throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_COMPRESS_GZIP_HPP__  */




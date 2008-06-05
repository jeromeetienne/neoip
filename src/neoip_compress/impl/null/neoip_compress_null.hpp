/*! \file
    \brief Declaration of the compress_null_t
*/


#ifndef __NEOIP_COMPRESS_NULL_HPP__ 
#define __NEOIP_COMPRESS_NULL_HPP__ 
/* system include */
/* local include */
#include "neoip_compress_vapi.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief null implementation of the compress_t API
 */
class compress_null_t : public compress_vapi_t {
private:
public:
	// list of inherited virtual function from \ref compress_vapi_t
	datum_t		compress(const datum_t &datum, size_t max_len)		throw();
	datum_t		uncompress(const datum_t &datum, size_t max_len)	throw();
	
	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(compress_vapi_t, compress_null_t);
	
	static void	factory_domain_insert()					throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_COMPRESS_NULL_HPP__  */




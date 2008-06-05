/*! \file
    \brief Definition of the \ref compress_layer_t
*/

/* system include */
/* local include */
#include "neoip_compress_layer.hpp"
#include "neoip_namespace.hpp"
#include "neoip_log.hpp"

// list of include for the factory_product_t insertion
#include "neoip_compress_null.hpp"
#include "neoip_compress_deflate.hpp"
#include "neoip_compress_gzip.hpp"
#include "neoip_compress_zlib.hpp"
#include "neoip_compress_bzip.hpp"

NEOIP_NAMESPACE_BEGIN

// define the factory plant
FACTORY_PLANT_DEFINITION (compress_factory, compress_type_t, compress_vapi_t);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 		CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
compress_layer_t::compress_layer_t()	throw()
{
	// insert all the slay_*_t factory_product_t
	compress_null_t		::factory_domain_insert();
	compress_deflate_t	::factory_domain_insert();
	compress_gzip_t		::factory_domain_insert();
	compress_zlib_t		::factory_domain_insert();
	compress_bzip_t		::factory_domain_insert();
}

/** \brief Destructor
 */
compress_layer_t::~compress_layer_t()	throw()
{
	
}



NEOIP_NAMESPACE_END


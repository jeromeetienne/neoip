/*! \file
    \brief Header of the \ref compress_layer_t
*/


#ifndef __NEOIP_COMPRESS_LAYER_HPP__
#define __NEOIP_COMPRESS_LAYER_HPP__
/* system include */
/* local include */
#include "neoip_compress_vapi.hpp"
#include "neoip_compress_layer_init.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief This class handles global stuff in the \ref compress_full_t and co
 */
class compress_layer_t {
public:
	/*************** ctor/dtor	***************************************/
	compress_layer_t()		throw();
	~compress_layer_t()		throw();
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_COMPRESS_LAYER_HPP__



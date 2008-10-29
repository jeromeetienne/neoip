/*! \file
    \brief Header of the amf0_build_t class

*/


#ifndef __NEOIP_AMF0_BUILD_HPP__
#define __NEOIP_AMF0_BUILD_HPP__
/* system include */
#include <sstream>
/* local include */
#include "neoip_bytearray.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	dvar_t;

/** \brief helper for the dvar_t object
 */
class amf0_build_t {
private:
public:
	static void		to_amf0(const dvar_t &dvar, bytearray_t &amf0_data)	throw();
	static bytearray_t	to_amf0(const dvar_t &dvar)				throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_AMF0_BUILD_HPP__  */




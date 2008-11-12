/*! \file
    \brief Header of the rtmp_build_t class

*/


#ifndef __NEOIP_RTMP_BUILD_HPP__
#define __NEOIP_RTMP_BUILD_HPP__
/* system include */
#include <sstream>
/* local include */
#include "neoip_bytearray.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_pkthd_t;

/** \brief helper for the dvar_t object
 */
class rtmp_build_t {
private:
public:
	static void		serialize(const rtmp_pkthd_t &rtmp_pkthd, const datum_t &rtmp_body
									, bytearray_t &bytearray)	throw();
	static void		serialize(const rtmp_pkthd_t &rtmp_pkthd, const bytearray_t &rtmp_body
									, bytearray_t &bytearray)	throw();
	static bytearray_t	serialize(const rtmp_pkthd_t &rtmp_pkthd, const datum_t &rtmp_body)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_BUILD_HPP__  */




/*! \file
    \brief Header of the neoip_id
    
*/


#ifndef __NEOIP_PKT_HPP__ 
#define __NEOIP_PKT_HPP__ 
/* system include */
/* local include */
#include "neoip_bytearray.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

#if 1
NEOIP_SERIAL_INHERITANCE_START	(pkt_t);
NEOIP_SERIAL_INHERITANCE_END	(pkt_t);
#endif

#if 0
/** \brief class definition for id
 */
class pkt_t : public bytearray_t {
private:
public:
	/*************** ctor/dtor	***************************************/
	pkt_t(const size_t reserved_size = 1024	, const size_t start_offset = 512
			, const size_t grow_chunk_len = 256)
			throw() : bytearray_t(reserved_size, start_offset, grow_chunk_len)	{}
	pkt_t(const void *data, int len)	throw() : bytearray_t(data, len)		{}
	pkt_t(const datum_t &datum)		throw() : bytearray_t(datum)			{}
};
#endif

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_PKT_HPP__  */











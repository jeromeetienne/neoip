/*! \file
    \brief Declaration of static helper functions for ipport_aview_t
    
*/


#ifndef __NEOIP_ENDIANSWAP_HPP__ 
#define __NEOIP_ENDIANSWAP_HPP__ 
/* system include */
/* local include */
#include "neoip_ipport_aview.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief static helpers for ipport_aview_t
 */
class endianswap_t {
public:
	/** \brief This function swap the endianness of a uint32_t value
	 * 
	 * - WARNING: it swap no matter the host endianess
	 *   - aka this is not htonl/ntohl
	 */ 
	static uint32_t	swap32(uint32_t value)	throw()
	{
		return	((value & 0xff000000) >> 24)	|
			((value & 0x00ff0000) >>  8)	|
			((value & 0x0000ff00) <<  8)	|
			((value & 0x000000ff) << 24);
	}

	/** \brief This function swap the endianness of a uint16_t value
	 * 
	 * - WARNING: it swap no matter the host endianess
	 *   - aka this is not htonl/ntohl
	 */ 
	static uint16_t	swap16(uint16_t value)	throw()
	{
		return	((value & 0xff00) >> 8)	|
			((value & 0x00ff) << 8);
	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ENDIANSWAP_HPP__  */











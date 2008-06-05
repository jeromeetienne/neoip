/*! \file
    \brief Header of the \ref serial_t
    
*/


#ifndef __NEOIP_ENDIAN_HPP__
#define __NEOIP_ENDIAN_HPP__
/* system include */
#include <iostream>
#ifdef _WIN32
// this include is doing a LOT of dirty things!!!!!
//#	include <windows.h>
// TODO to study this horrible stuff
// - in /usr/i586-mingw32msvc/include/wingdi.h there is a #define ERROR 0
// - beuark this is a workaround
//#	undef ERROR
//#	undef INFINITE
#else
#	include <netinet/in.h>		// for htons etc... in the inline serialization
#endif
/* local include */


NEOIP_NAMESPACE_BEGIN

#define	NEOIP_BSWAP16(x) (uint16_t) 		\
	(((x) >> 8) | ((x) << 8))

#define	NEOIP_BSWAP32(x)	(uint32_t) 	\
	(((x) >> 24) | (((x) >> 8) & 0xff00) |	\
	(((x) << 8) & 0xff0000) | ((x) << 24))

#define	NEOIP_BSWAP64(x)	(uint64_t) 						\
	(((x) >> 56) | (((x) >> 40) & 0xff00) | (((x) >> 24) & 0xff0000) |	\
	(((x) >> 8) & 0xff000000) | (((x) << 8) & ((uint64_t)0xff << 32)) |	\
	(((x) << 24) & ((uint64_t)0xff << 40)) |				\
	(((x) << 40) & ((uint64_t)0xff << 48)) | (((x) << 56)))

#if _BYTE_ORDER == _LITTLE_ENDIAN
#	define NEOIP_HTOBE16(x)		NEOIP_BSWAP16(x)
#	define NEOIP_HTOBE32(x)		NEOIP_BSWAP32(x)
#	define NEOIP_HTOBE64(x)		NEOIP_BSWAP64(x)
#	define NEOIP_BETOH16(x)		NEOIP_BSWAP16(x)
#	define NEOIP_BETOH32(x)		NEOIP_BSWAP32(x)
#	define NEOIP_BETOH64(x)		NEOIP_BSWAP64(x)
#else	/* _BYTE_ORDER != _LITTLE_ENDIAN */
#	define NEOIP_HTOBE16(x)		((uint16_t)(x))
#	define NEOIP_HTOBE32(x)		((uint32_t)(x))
#	define NEOIP_HTOBE64(x)		((uint64_t)(x))
#	define NEOIP_BETOH16(x)		((uint16_t)(x))
#	define NEOIP_BETOH32(x)		((uint32_t)(x))
#	define NEOIP_BETOH64(x)		((uint64_t)(x))
#endif

#define NEOIP_HTONS(x)	NEOIP_HTOBE16(x)
#define NEOIP_NTOHS(x)	NEOIP_BTTOH16(x)
#define NEOIP_HTONL(x)	NEOIP_HTOBE32(x)
#define NEOIP_NTOHL(x)	NEOIP_BTTOH32(x)

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_ENDIAN_HPP__  */


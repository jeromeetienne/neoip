/*! \file
    \brief Just an indirection to include gcrypt without leaving crub

- e.g. gcrypt when compiled with mingw32 leave #define on name i use
  - it is due to mingw32 and likely due to mswin in turn
- in anycase never include it directly, predere to include this head

*/


#ifndef __NEOIP_CRYPTO_GCRYPT_HPP__ 
#define __NEOIP_CRYPTO_GCRYPT_HPP__ 

/* system include */
#include <gcrypt.h>
#ifdef _WIN32
#	undef ERROR
#	undef INFINITE
#	undef OPTIONAL
#endif

#endif	/* __NEOIP_CRYPTO_GCRYPT_HPP__  */


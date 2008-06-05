/*! \file
    \brief Some definition to help the interaction between C and C++
    
*/


#ifndef __NEOIP_CPP_EXTERN_C_HPP__ 
#define __NEOIP_CPP_EXTERN_C_HPP__ 
/* system include */

#ifdef __cplusplus
#	define	NEOIP_CPP_EXTERN_C_BEGIN	extern "C" {
#	define	NEOIP_CPP_EXTERN_C_END		}
#else
#	define	NEOIP_CPP_EXTERN_C_BEGIN
#	define	NEOIP_CPP_EXTERN_C_END
#endif

#endif	/* __NEOIP_CPP_EXTERN_C_HPP__  */




/*! \file
    \brief Header of the neoip_namespace

*/


#ifndef __NEOIP_NAMESPACE_HPP__ 
#define __NEOIP_NAMESPACE_HPP__ 

/* system include */
/* local include */

#if 0
#	define NOTHROW			throw()
#else
#	define NOTHROW
#endif

// not really namespace - just some experiment with the function attribute
#define	NATTR_DEPRECIATED	__attribute__ ((deprecated))
#define	NATTR_RESULT_NOIGN	__attribute__ ((warn_unused_result))

#define NEOIP_NAMESPACE_BEGIN	namespace neoip {
#define NEOIP_NAMESPACE_END	}


#endif	/* __NEOIP_NAMESPACE_HPP__  */




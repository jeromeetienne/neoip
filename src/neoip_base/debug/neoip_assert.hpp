/*! \file
    \brief Header of the neoip_assert
    
*/


#ifndef __NEOIP_ASSERT_HPP__ 
#define __NEOIP_ASSERT_HPP__ 
/* system include */
#include <assert.h>
/* local include */
#include "neoip_stdlog.hpp"
#include "neoip_cpp_backtrace.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief global assert - all neoip assert are using this one
 * 
 * - just a normal assert with a backtrace display to avoid to launch a debugger
 *   to get the stack backtrace
 */
#define NEOIP_ASSERT( x )								\
	do{										\
		if( !(x) ){								\
			KLOG_STDOUT( __FILE__ << ":" << __LINE__ << ":" << __func__	\
				<< "() : ASSERT FAILED on condition (" << #x << ")\n");	\
			KLOG_STDOUT("backtrace=" << cpp_backtrace_t().initialize());	\
			assert(x);							\
		}									\
	}while(0)
	
/** \brief assert that depends only on local 'process' condition
 */
#if NO_DBG_ASSERT
#	define DBG_ASSERT( x )		if(x){}
#else
#	define DBG_ASSERT( x )		NEOIP_ASSERT(x)
#endif

/** \brief assert that depends on network condition 
 * 
 * - typically in a packet parsing, some field are supposed to be in a given format and 
 *   an assert should be triggered if they are not
 *   - but to deploy code with such assert will open a lot of DoS
 *   - so DBGNET_ASSERT is like a DBG_ASSERT + this additionnal condition
 */
#if NO_DBG_ASSERT
#	define DBGNET_ASSERT( x )	if(!(x)){}
#else
#	define DBGNET_ASSERT( x )	NEOIP_ASSERT(x)
#endif

// KLOG_ERR("DBGNET condition " << #x << " failed. going on")

/** \brief assert that depends on external conditions
 * 
 * - This assert will always stays, it is permanent
 * - no compilation option will disable it
 */
#define EXP_ASSERT( x )			NEOIP_ASSERT(x)

NEOIP_NAMESPACE_END

#endif // __NEOIP_ASSERT_HPP__ 

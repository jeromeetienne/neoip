/*! \file
    \brief Declaration of the \ref log_layer_t

*/


#ifndef __NEOIP_LOG_HPP__ 
#define __NEOIP_LOG_HPP__ 
/* system include */
/* local include */
#include "neoip_log_level.hpp"
#include "neoip_log_layer.hpp"
#include "neoip_namespace.hpp"

#include "neoip_stdlog.hpp"	// add the KLOG_STDOUT/KLOG_STDERR for convenience

NEOIP_NAMESPACE_BEGIN;

#ifndef KLOG_CATEGORY_NAME
#	define KLOG_CATEGORY_NAME	__FILE__
#endif // KLOG_CATEGORY_NAME



#define KLOG_LEVEL( klog_level, klog_data )							\
	do {											\
		log_layer_t *log_layer = get_global_log_layer();				\
		if( log_level_t(klog_level) <= log_layer->find_category_level(KLOG_CATEGORY_NAME) ){ \
			std::ostringstream	oss_klog_level;					\
			oss_klog_level << klog_data;						\
			log_layer->do_log(  __FILE__, __LINE__, (char *)__func__		\
				, KLOG_CATEGORY_NAME, klog_level, oss_klog_level.str() );	\
		}										\
	}while(0)

#define KLOG_EMERG( nlog_data )		KLOG_LEVEL( log_level_t::EMERG	, nlog_data )
#define KLOG_ALERT( nlog_data )		KLOG_LEVEL( log_level_t::ALERT	, nlog_data )
#define KLOG_CRITICAL( nlog_data )	KLOG_LEVEL( log_level_t::CRIT	, nlog_data )
#define KLOG_ERROR( nlog_data )		KLOG_LEVEL( log_level_t::ERR	, nlog_data )
#define KLOG_WARNING( nlog_data )	KLOG_LEVEL( log_level_t::WARNING, nlog_data )
#define KLOG_NOTICE( nlog_data )	KLOG_LEVEL( log_level_t::NOTICE	, nlog_data )
#define KLOG_INFO( nlog_data )		KLOG_LEVEL( log_level_t::INFO	, nlog_data )

#if NO_DBG_TRACE
#	define KLOG_DEBUG( nlog_data )		do{}while(0)
#else
#	define KLOG_DEBUG( nlog_data )		KLOG_LEVEL( log_level_t::DEBUG	, nlog_data )
#endif

#define KLOG_CRIT( nlog_data )		KLOG_CRITICAL( nlog_data )
#define KLOG_ERR( nlog_data )		KLOG_ERROR( nlog_data )
#define KLOG_WARN( nlog_data )		KLOG_WARNING( nlog_data )
#define KLOG_DBG( nlog_data )		KLOG_DEBUG( nlog_data )

//! special log function to easily display expression/variable content
// - e.g. KLOG_VAR( slota_db.size() );
#define KLOG_VAR( expr )		KLOG_ERR( "("<<#expr << ")=" << (expr) )

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LOG_HPP__  */






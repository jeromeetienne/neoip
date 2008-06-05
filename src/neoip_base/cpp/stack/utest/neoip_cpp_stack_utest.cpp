/*! \file
    \brief Definition of the unit test for the \ref cpp_stack_t and co

\par Brief description
This module performs an unit test for the cpp_stack_t classes

*/

/* system include */
/* local include */
#include "neoip_cpp_stack_utest.hpp"
#include "neoip_cpp_backtrace.hpp"
#include "neoip_cpp_location.hpp"
#include "neoip_log.hpp"

using namespace neoip;

/** \brief unit test for the cpp_stack_t class
 */
int neoip_cpp_stack_utest()
{
	int			n_error = 0;

	KLOG_ERR("full backtrace=" << cpp_backtrace_t().initialize() );
	KLOG_ERR("this function=" << cpp_backtrace_t().initialize()[0] );
	KLOG_ERR("previous function=" << cpp_backtrace_t().initialize()[1] );

	if( n_error )	KLOG_ERR("cpp_stack_product_t unit test FAILED!!!");
	else		KLOG_ERR("cpp_stack_product_t unit test PASSED!!!");
	return n_error;
}


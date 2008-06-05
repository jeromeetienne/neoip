/*! \file
    \brief Definition of the unit test for the \ref cpp_demangle_t and co

\par Brief description
This module performs an unit test for the cpp_demangle_t classes

*/

/* system include */
/* local include */
#include "neoip_cpp_demangle_utest.hpp"
#include "neoip_cpp_demangle.hpp"
#include "neoip_log.hpp"

using namespace neoip;

#include <cxxabi.h>

class utest_demangle_dummy {};

/** \brief unit test for the cpp_demangle_product_t class
 */
int neoip_cpp_demangle_utest()
{
	int			n_error = 0;

	KLOG_ERR("normal rtti name=" << typeid(utest_demangle_dummy).name() );
	KLOG_ERR("the same demangled=" << neoip_cpp_demangle_type(typeid(utest_demangle_dummy).name()));
	int status;
	KLOG_ERR("test=" <<abi::__cxa_demangle(typeid(utest_demangle_dummy).name(), 0, 0, &status));

	if( n_error )	KLOG_ERR("cpp_demangle_product_t unit test FAILED!!!");
	else		KLOG_ERR("cpp_demangle_product_t unit test PASSED!!!");
	return n_error;
}


/*! \file
    \brief Definition of the unit test for the \ref version_t and co

*/

/* system include */
#include <map>
/* local include */
#include "neoip_version_utest.hpp"
#include "neoip_version.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

using namespace neoip;

/** \brief unit test for the version_t class
 */
int neoip_version_utest()
{
	int		n_error = 0;
	version_t	version_unserial;
	serial_t	serial;

	version_t	version1 = "1.2";
	version_t	version2 = "0.99";
	
	KLOG_ERR("version1=" << version1);
	KLOG_ERR("version2=" << version2);
	
	// test is_compatible()
	if( version1.is_compatible("2.1") ){
		KLOG_ERR("version.is_compatible() is BUGGED!");
		goto error;
	}
	if( version1.is_compatible("1.5") == false ){
		KLOG_ERR("version.is_compatible() is BUGGED!");
		goto error;
	}
	

// test serialization consitency
	serial << version1;
	serial >> version_unserial;
	if( version1 != version_unserial ){
		KLOG_ERR("version_t serialization is INCONSISTENT!!! BUG!!!");
		goto error;
	}

	if( n_error )	goto error;
	KLOG_ERR("version_t unit test PASSED!!!");

	return n_error;
error:;	KLOG_ERR("version_t unit test FAILED!!!");
	return 1;
}


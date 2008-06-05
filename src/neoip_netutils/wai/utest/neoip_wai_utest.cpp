/*! \file
    \brief Definition of the unit test for the \ref neoip_wai and co

\par Brief description
This module performs an unit test for the neoip_wai classes

*/

/* system include */
/* local include */
#include "neoip_wai_utest.hpp"
#include "neoip_wai.hpp"
#include "neoip_log.hpp"

using namespace neoip;
/** \brief unit test for the neoip_wai class
 */
int neoip_wai_utest()
{
	wai32_t	wai1, wai2, wai3, wai4;
	int		n_error = 0;
	KLOG_ERR("neoip_wai Unit test");

	wai1	= (int32_t)0;
	wai2	= wai2.get_max_diff();	

	// verify that 0 is less than max positive value
	if( !(wai1 < wai2) ){
		KLOG_ERR( "neoip_wai Unit Test: " << wai1 << " is NOT less than " << wai2
						<< " BUT it SHOULD be. BUG!!!");
		n_error++;
	}

	// increase wai2 by 2
	// - so it goes from the maximum positive value to the minimal negative value
	wai2	+= 3;

	DBG_ASSERT( (wai1 < wai2) == (wai2 > wai1) );
	// verify that 0 is more than min negative value
	if( !(wai1 > wai2) ){
		KLOG_ERR( "neoip_wai Unit Test: " << wai1 << " is NOT greater than " << (int32_t)wai2
						<< " BUT it SHOULD be. BUG!!!");
		n_error++;
	}

	if( n_error )	KLOG_ERR("neoip_wai unit test FAILED!!!");
	else		KLOG_ERR("neoip_wai unit test PASSED!!!");
	return n_error;
}


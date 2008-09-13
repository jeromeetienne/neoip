/*! \file
    \brief Definition of the unit test for the \ref neoip_crypto_slidwin

\par Brief description
This module performs an unit test for the neoip_crypto_slidwin classes

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_slidwin_utest.hpp"
#include "neoip_slidwin.hpp"
#include "neoip_wai.hpp"
#include "neoip_log.hpp"

using namespace neoip;


/** \brief unit test for the neoip_slidwin class
 */
int neoip_slidwin_utest()
{
	int				n_error = 0;
	wai32_t				seqnb	= 0;
	slidwin_t<int, wai32_t>		slidwin(5, 0);
// TODO a good way to test this slidwin is to have a long array
// and to push its value in a slidwin, and then to check if the slidwin is equal to the
// long array chunk	

	seqnb = seqnb.get_max_diff() - 5;
#if 0
	for( int i = 1; i < 15; i++ ){
		slidwin.set( seqnb, i );
		seqnb += 2;

		std::stringstream	sstream;
		for( wai32_t idx = slidwin.get_index_first(); idx <= slidwin.get_index_last(); idx++ )
			sstream << " " << std::hex << slidwin.get(idx);
		KLOG_ERR( std::hex << i  << "=>" << sstream.str() );
	}
#endif

	if( n_error )	goto error;
	KLOG_ERR("SLIDWIN_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("SLIDWIN_UTEST FAILED!!!!");
	return -1;
}

